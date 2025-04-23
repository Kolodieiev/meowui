#include "lua_i2c.h"

const struct luaL_Reg LIB_I2C[] = {
    // {STR_MCU_MILLIS, lua_mcu_millis},
    {nullptr, nullptr},
};

int lua_open_i2c(lua_State *L)
{
    luaL_newlib(L, LIB_I2C);
    return 1;
}