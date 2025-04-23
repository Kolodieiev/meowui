#include "lua_net.h"

const struct luaL_Reg LIB_NET[] = {
    // {STR_MCU_MILLIS, lua_mcu_millis},
    {nullptr, nullptr},
};

int lua_open_net(lua_State *L)
{
    luaL_newlib(L, LIB_NET);
    return 1;
}