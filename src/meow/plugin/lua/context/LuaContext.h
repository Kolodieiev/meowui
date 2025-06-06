#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"

#include "../lua.h"

namespace meow
{
    class LuaContext : public IContext
    {
    public:
        LuaContext();
        virtual ~LuaContext();

        bool execScript(const char *lua_script);
        String getMsg() const { return _msg; }

    protected:
        virtual bool loop() override;
        virtual void update() override;
        //
    private:
        bool _is_script_exec = false;
        lua_State *_lua{nullptr};
        String _msg;
        uint16_t _hook_counter{0};
        //----------------------------------------------------------------------------------
        Notification *_notification{nullptr};
        //----------------------------------------------------------------------------------
        std::vector<uint16_t> _loaded_img_id;
        //----------------------------------------------------------------------------------
        std::vector<IWidget *> _managed_widgets;
        //----------------------------------------------------------------------------------
        static const LuaRegisterFunc LIB_REGISTER_FUNCS[];
        static const struct luaL_Reg LIB_CONTEXT[];
        static const struct luaL_Reg LIB_INPUT[];
        //----------------------------------------------------------------------------------
        static LuaContext *_self;
        //----------------------------------------------------------------------------------

        static void *luAlloc(void *ud, void *ptr, size_t osize, size_t nsize);
        static void luaHook(lua_State *L, lua_Debug *ar);
        //
        bool initLua();
        void luaErrToMsg();
        bool hasLuaFunction(const char *func_name);
        int callLuaFunction(const char *func_name);

        //---------------------------------------------------------------------------------- reg

        static int lua_register_context(lua_State *L);
        static int lua_register_input(lua_State *L);

        //---------------------------------------------------------------------------------- context

        static int lua_context_exit(lua_State *L);
        static int lua_context_get_layout(lua_State *L);
        static int lua_context_manage_widget(lua_State *L);

        //---------------------------------------------------------------------------------- input

        static int lua_input_enable_btn(lua_State *L);
        static int lua_input_disable_btn(lua_State *L);
        static int lua_input_is_holded(lua_State *L);
        static int lua_input_is_pressed(lua_State *L);
        static int lua_input_is_released(lua_State *L);
        static int lua_input_lock(lua_State *L);

        //---------------------------------------------------------------------------------- helper

        static int lua_init_type(lua_State *L);
        static int lua_unrequire(lua_State *L);
        static int lua_show_toast(lua_State *L);
        static int lua_show_notification(lua_State *L);
        static int lua_hide_notification(lua_State *L);
        static int lua_load_img(lua_State *L);
        static int lua_delete_img(lua_State *L);
    };
}