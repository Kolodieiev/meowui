#pragma once

extern "C"
{
#include "./lua_src/lua.h"
#include "./lua_src/lualib.h"
#include "./lua_src/lauxlib.h"
}

#include "./lua_err_msg.h"

typedef int (*LuaRegisterFunc)(lua_State *L);