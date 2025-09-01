#pragma GCC optimize("O3")
#include "lua_wifi.h"

const struct luaL_Reg LIB_WIFI[] = {
    // {STR_MCU_MILLIS, lua_mcu_millis},
    {nullptr, nullptr},
};

int lua_open_wifi(lua_State *L)
{
    luaL_newlib(L, LIB_WIFI);
    return 1;
}