#pragma GCC optimize("O3")
#include "LuaContext.h"
#include "esp_heap_caps.h"
#include "meow/ui/widget/layout/EmptyLayout.h"

#include "../lua_lib/custom_register.h"
#include "../lua_lib/custom_searcher.h"

const char STR_NOT_ENOUGH_RAM[] = "Недостатньо RAM для роботи LuaVM";
const char STR_LUA_ERR[] = "Помилка в скрипті Lua: ";
const char STR_LUAVM_CREATE_ERR[] = "Помилка створення LuaVM";
const char STR_ERR_SCRIPT_STRUCT[] = "Скрипт повинен містити визначення функції update";
//
const char STR_UNREQUIRE_NAME[] = "unrequire";
//
const char STR_UPDATE_NAME[] = "update";
//
const char STR_LIB_NAME_CONTEXT[] = "context";
const char STR_FUNC_CONTEXT_EXIT[] = "exit";
//
const char STR_LIB_NAME_INPUT[] = "input";
const char STR_FUNC_INPUT_ENABLE[] = "enable_btn";
const char STR_FUNC_INPUT_DISABLE[] = "disable_btn";
const char STR_FUNC_INPUT_IS_HOLDED[] = "is_holded";
const char STR_FUNC_INPUT_IS_PRESSED[] = "is_pressed";
const char STR_FUNC_INPUT_IS_RELEASED[] = "is_released";
const char STR_FUNC_INPUT_LOCK[] = "lock";

namespace meow
{
    const LuaRegisterFunc LuaContext::LIB_REGISTER_FUNCS[] = {
        LuaContext::lua_register_context,
        LuaContext::lua_register_input,
    };

    const struct luaL_Reg LuaContext::LIB_CONTEXT[] = {
        {STR_FUNC_CONTEXT_EXIT, LuaContext::lua_context_exit},
        {nullptr, nullptr},
    };

    const struct luaL_Reg LuaContext::LIB_INPUT[] = {
        {STR_FUNC_INPUT_ENABLE, lua_input_enable_btn},
        {STR_FUNC_INPUT_DISABLE, lua_input_disable_btn},
        {STR_FUNC_INPUT_IS_HOLDED, lua_input_is_holded},
        {STR_FUNC_INPUT_IS_PRESSED, lua_input_is_pressed},
        {STR_FUNC_INPUT_IS_RELEASED, lua_input_is_released},
        {STR_FUNC_INPUT_LOCK, lua_input_lock},
        {nullptr, nullptr},
    };

    LuaContext *LuaContext::_this_ptr;

    LuaContext::LuaContext()
    {
        EmptyLayout *layout = new EmptyLayout(1);
        layout->setBackColor(TFT_BLUE);
        layout->setWidth(_display.width());
        layout->setHeight(_display.height());
        setLayout(layout);

        _this_ptr = this;
    }

    LuaContext::~LuaContext()
    {
        lua_close(_lua);
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
        lua_register(_lua, STR_UNREQUIRE_NAME, lua_unrequire);

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
            _msg = STR_ERR_SCRIPT_STRUCT;
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

            _this_ptr->_msg = STR_NOT_ENOUGH_RAM;
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

        if (_this_ptr->_hook_counter > 12)
        {
            _this_ptr->_hook_counter = 0;
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

        ++_this_ptr->_hook_counter;
    }

    void LuaContext::luaErrToMsg()
    {
        const char *err_msg = lua_tostring(_lua, -1);
        if (err_msg)
        {
            _msg = STR_LUA_ERR;
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
        lua_setglobal(L, STR_LIB_NAME_CONTEXT);
        return 0;
    }

    int LuaContext::lua_register_input(lua_State *L)
    {
        luaL_newlib(L, LIB_INPUT);
        lua_setglobal(L, STR_LIB_NAME_INPUT);
        return 0;
    }

    //---------------------------------------------------------------------------------------------------- context

    int LuaContext::lua_context_exit(lua_State *L)
    {
        _this_ptr->release();
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

    int LuaContext::lua_unrequire(lua_State *L)
    {
        const char *libname = luaL_checkstring(L, 1);

        lua_getglobal(L, "package");
        lua_getfield(L, -1, "loaded");
        lua_getfield(L, -1, libname); // stack: package, loaded, module

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
        lua_setfield(L, -2, libname);

        lua_pop(L, 2);

        lua_pushnil(L);
        lua_setglobal(L, libname);

        return 0;
    }
}