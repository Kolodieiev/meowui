#pragma GCC optimize("O3")
#include "lua_uart.h"

const struct luaL_Reg LIB_UART[] = {
    // {STR_MCU_MILLIS, lua_mcu_millis},
    {nullptr, nullptr},
};

int lua_open_uart(lua_State *L)
{
    luaL_newlib(L, LIB_UART);
    return 1;
}