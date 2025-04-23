#include "lua_sd.h"
#include <Arduino.h>

const char STR_SD_PRINT_TEST[] = "test";

int lua_sd_test(lua_State *L)
{
    log_i("Test require succsses");
    return 0;
}

const struct luaL_Reg LIB_SD[] = {
    {STR_SD_PRINT_TEST, lua_sd_test},
    {nullptr, nullptr},
};

int lua_open_sd(lua_State *L)
{
    luaL_newlib(L, LIB_SD);
    return 1;
}