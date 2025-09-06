#pragma GCC optimize("O3")
#include "LuaContext.h"
#include "esp_heap_caps.h"
#include "meow/ui/widget/layout/EmptyLayout.h"

#include "../lua_lib/custom_register.h"
#include "../lua_lib/custom_searcher.h"
#include "../lua_lib/custom_type_initializer.h"

#include "../lua_lib/type/widget/lua_iwidget_cont.h"

#include "meow/manager/ResManager.h"
#include "meow/plugin/lua/lua_lib/helper/lua_helper.h"

const char STR_NOTIFICATION[] = "Повідомлення";
const char STR_OK[] = "OK";
const char STR_NOT_ENOUGH_RAM[] = "Недостатньо RAM для роботи LuaVM";
//
const char STR_UPDATE_NAME[] = "update";
//
namespace meow
{
    const LuaRegisterFunc LuaContext::LIB_REGISTER_FUNCS[] = {
        LuaContext::lua_register_context,
        LuaContext::lua_register_input,
    };

    const struct luaL_Reg LuaContext::LIB_CONTEXT[] = {
        {"manageWidget", LuaContext::lua_context_manage_widget},
        {"exit", LuaContext::lua_context_exit},
        {"getLayout", LuaContext::lua_context_get_layout},
        {nullptr, nullptr},
    };

    const struct luaL_Reg LuaContext::LIB_INPUT[] = {
        {"enable_btn", lua_input_enable_btn},
        {"disable_btn", lua_input_disable_btn},
        {"is_holded", lua_input_is_holded},
        {"is_pressed", lua_input_is_pressed},
        {"is_released", lua_input_is_released},
        {"lock", lua_input_lock},
        {nullptr, nullptr},
    };

    LuaContext *LuaContext::_self;

    LuaContext::LuaContext()
    {
#ifdef GRAPHICS_ENABLED
        EmptyLayout *layout = new EmptyLayout(1);
        layout->setBackColor(TFT_BLUE);
        layout->setWidth(_display.width());
        layout->setHeight(_display.height());
        setLayout(layout);

        _notification = new Notification(1);
#endif

        _self = this;
    }

    LuaContext::~LuaContext()
    {
        lua_close(_lua);
        reset_custom_initializer();
        delete _notification;

        for (size_t i = 0; i < _loaded_img_id.size(); ++i)
            _res.deleteBmpRes(_loaded_img_id[i]);

        for (size_t i = 0; i < _managed_widgets.size(); ++i)
            delete _managed_widgets[i];
    }

    bool LuaContext::initLua()
    {
        if (_lua)
        {
            _is_script_exec = false;
            lua_close(_lua);
        }

        _lua = lua_newstate(luAlloc, nullptr);

        if (!_lua)
            return false;

        luaL_openlibs(_lua);
        lua_sethook(_lua, luaHook, LUA_MASKCOUNT, 10000);

        const size_t funcs_num = sizeof(LIB_REGISTER_FUNCS) / sizeof(LuaRegisterFunc);
        for (size_t i = 0; i < funcs_num; ++i)
            LIB_REGISTER_FUNCS[i](_lua);

        register_custom_modules(_lua);
        register_custom_searcher(_lua);

        lua_register(_lua, "initType", lua_init_type);
        lua_register(_lua, "unrequire", lua_unrequire);

        lua_register(_lua, "showToast", lua_show_toast);
        lua_register(_lua, "showNotification", lua_show_notification);
        lua_register(_lua, "hideNotification", lua_hide_notification);

        lua_register(_lua, "loadImg", lua_load_img);
        lua_register(_lua, "deleteImg", lua_delete_img);

        _msg = "";
        return true;
    }

    bool LuaContext::execScript(const char *lua_script)
    {
        if (!initLua())
            return false;

        if (luaL_loadstring(_lua, lua_script) != LUA_OK || lua_pcall(_lua, 0, LUA_MULTRET, 0) != LUA_OK)
        {
            luaErrToMsg();
            return false;
        }
        else if (!hasLuaFunction(STR_UPDATE_NAME))
        {
            _msg = "Скрипт повинен містити визначення функції update";
            return false;
        }
        else
        {
            _msg = "";
            _is_script_exec = true;
            return true;
        }
    }

    void *LuaContext::luAlloc(void *ud, void *ptr, size_t osize, size_t nsize)
    {
        (void)ud;
        (void)osize;

        if (nsize == 0)
        {
            free(ptr);
            return nullptr;
        }
        else
        {
            size_t free_mem = heap_caps_get_free_size(MALLOC_CAP_8BIT);

            if (nsize < free_mem && free_mem - nsize > 40 * 1024)
            {
                return realloc(ptr, nsize);
            }
            else if (psramInit())
            {
                free_mem = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);

                if (free_mem > nsize)
                    return ps_realloc(ptr, nsize);
            }

            _self->_msg = STR_NOT_ENOUGH_RAM;
            log_e("%s", STR_NOT_ENOUGH_RAM);
            return nullptr;
        }
    }

    void LuaContext::luaHook(lua_State *L, lua_Debug *ar)
    {
        (void)L;
        (void)ar;

        int used_kb = lua_gc(L, LUA_GCCOUNT, 0);
        if (used_kb > 30)
            lua_gc(L, LUA_GCCOLLECT, 0);

        if (_self->_hook_counter > 12)
        {
            _self->_hook_counter = 0;
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

        ++_self->_hook_counter;
    }

    void LuaContext::luaErrToMsg()
    {
        const char *err_msg = lua_tostring(_lua, -1);
        if (err_msg)
        {
            _msg = "Помилка в скрипті Lua: ";
            _msg += err_msg;
        }
    }

    bool LuaContext::hasLuaFunction(const char *func_name)
    {
        lua_getglobal(_lua, func_name);
        bool exists = lua_isfunction(_lua, -1);
        lua_pop(_lua, 1);
        return exists;
    }

    int LuaContext::callLuaFunction(const char *func_name)
    {
        lua_getglobal(_lua, func_name);
        return lua_pcall(_lua, 0, 0, 0);
    }

    bool LuaContext::loop()
    {
        return true;
    }

    void LuaContext::update()
    {
        if (_is_script_exec)
        {
            if (callLuaFunction(STR_UPDATE_NAME) != LUA_OK)
            {
                luaErrToMsg();
                release();
            }
        }
    }

    //----------------------------------------------------------------------------------------------------

    int LuaContext::lua_register_context(lua_State *L)
    {
        luaL_newlib(L, LIB_CONTEXT);
        lua_setglobal(L, "context");
        return 0;
    }

    int LuaContext::lua_register_input(lua_State *L)
    {
        luaL_newlib(L, LIB_INPUT);
        lua_setglobal(L, "input");
        return 0;
    }

    //---------------------------------------------------------------------------------------------------- context

    int LuaContext::lua_context_exit(lua_State *L)
    {
        _self->release();
        return 0;
    }

    int LuaContext::lua_context_get_layout(lua_State *L)
    {
        IWidgetContainer *layout = _self->getLayout();
        if (!layout)
        {
            lua_pushnil(L);
        }
        else
        {
            IWidgetContainer **udata = (IWidgetContainer **)lua_newuserdata(L, sizeof(IWidgetContainer *));
            *udata = layout;

            luaL_getmetatable(L, STR_TYPE_NAME_IWIDGET_CONT);
            lua_setmetatable(L, -2);
        }

        return 1;
    }

    int LuaContext::lua_context_manage_widget(lua_State *L)
    {
        Label **label = (Label **)lua_touserdata(L, 1);
        _self->_managed_widgets.push_back(*label);
        return 0;
    }

    //---------------------------------------------------------------------------------------------------- input

    int LuaContext::lua_input_enable_btn(lua_State *L)
    {
        int btn = luaL_checkinteger(L, 1);
        _input.enableBtn((BtnID)btn);
        return 0;
    }

    int LuaContext::lua_input_disable_btn(lua_State *L)
    {
        int btn = luaL_checkinteger(L, 1);
        _input.disableBtn((BtnID)btn);
        return 0;
    }

    int LuaContext::lua_input_is_holded(lua_State *L)
    {
        int btn = luaL_checkinteger(L, 1);
        lua_pushboolean(L, _input.isHolded((BtnID)btn));
        return 1;
    }

    int LuaContext::lua_input_is_pressed(lua_State *L)
    {
        int btn = luaL_checkinteger(L, 1);
        lua_pushboolean(L, _input.isPressed((BtnID)btn));
        return 1;
    }

    int LuaContext::lua_input_is_released(lua_State *L)
    {
        int btn = luaL_checkinteger(L, 1);
        lua_pushboolean(L, _input.isReleased((BtnID)btn));
        return 1;
    }

    int LuaContext::lua_input_lock(lua_State *L)
    {
        int btn = luaL_checkinteger(L, 1);
        int lock_time = luaL_checkinteger(L, 2);
        _input.lock((BtnID)btn, lock_time);
        return 0;
    }

    int LuaContext::lua_init_type(lua_State *L)
    {
        init_custom_type(L);
        return 0;
    }

    int LuaContext::lua_unrequire(lua_State *L)
    {
        const char *module_name = luaL_checkstring(L, 1);

        lua_getglobal(L, "package");
        lua_getfield(L, -1, "loaded");
        lua_getfield(L, -1, module_name); // stack: package, loaded, module

        if (!lua_isnil(L, -1))
        {
            lua_getfield(L, -1, "__unload");
            if (!lua_isfunction(L, -1))
            {
                lua_pop(L, 1);
            }
            else
            {
                lua_pushvalue(L, -2);
                if (lua_pcall(L, 1, 0, 0) != LUA_OK)
                {
                    const char *err = lua_tostring(L, -1);
                    log_e("Error in __unload: %s\n", err);
                    lua_pop(L, 1);
                }
            }
        }

        lua_pop(L, 1);

        lua_pushnil(L);
        lua_setfield(L, -2, module_name);

        lua_pop(L, 2);

        lua_pushnil(L);
        lua_setglobal(L, module_name);

        return 0;
    }

    int LuaContext::lua_show_toast(lua_State *L)
    {
        uint8_t arg_num = lua_gettop(L);

        const char *toast_str = luaL_checkstring(L, 1);
        float time = TOAST_LENGTH_SHORT;

        if (arg_num > 1)
            time = luaL_checknumber(L, 2);

        _self->showToast(toast_str, time);
        return 0;
    }

    int LuaContext::lua_show_notification(lua_State *L)
    {
        uint8_t arg_num = lua_check_top(L, {1, 4});
        if (arg_num == 0)
            return 0;

        if (arg_num == 1)
        {
            const char *notification_msg = luaL_checkstring(L, 1);
            _self->_notification->setTitleText(STR_NOTIFICATION);
            _self->_notification->setMsgText(notification_msg);
            _self->_notification->setLeftText(emptyString);
            _self->_notification->setRightText(STR_OK);
        }
        else if (arg_num == 4)
        {
            const char *n_title = luaL_checkstring(L, 1);
            const char *n_msg = luaL_checkstring(L, 2);
            const char *n_left = luaL_checkstring(L, 3);
            const char *n_rigt = luaL_checkstring(L, 4);

            _self->_notification->setTitleText(n_title);
            _self->_notification->setMsgText(n_msg);
            _self->_notification->setLeftText(n_left);
            _self->_notification->setRightText(n_rigt);
        }

        _self->showNotification(_self->_notification);
        return 0;
    }

    int LuaContext::lua_hide_notification(lua_State *L)
    {
        _self->hideNotification();
        return 0;
    }

    int LuaContext::lua_load_img(lua_State *L)
    {
        const char *path = luaL_checkstring(L, 1);
        uint16_t id = _res.loadBmpRes(path);

        if (id > 0)
            _self->_loaded_img_id.push_back(id);

        lua_pushinteger(L, id);
        return 1;
    }

    int LuaContext::lua_delete_img(lua_State *L)
    {
        uint16_t id = luaL_checkinteger(L, 1);

        for (auto it_b = _self->_loaded_img_id.begin(), it_e = _self->_loaded_img_id.end(); it_b != it_e; ++it_b)
        {
            if (*it_b == id)
            {
                _self->_loaded_img_id.erase(it_b);
                _res.deleteBmpRes(id);
                break;
            }
        }

        return 0;
    }
}