#pragma once

extern "C"
{
#include "./lua_src/lua.h"
#include "./lua_src/lualib.h"
#include "./lua_src/lauxlib.h"
}

typedef int (*LuaRegisterFunc)(lua_State *L);
typedef int (*LuaOpenFunc)(lua_State *L);