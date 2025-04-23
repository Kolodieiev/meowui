#include "lua_mcu.h"
#include <Arduino.h>

const char STR_LIB_NAME_MCU[] = "mcu";
//
const char STR_MCU_MILLIS[] = "millis";
const char STR_MCU_SLEEP[] = "sleep";

int lua_mcu_millis(lua_State *L)
{
    lua_pushinteger(L, millis());
    return 1;
}

int lua_mcu_sleep(lua_State *L)
{
    float ms = luaL_checknumber(L, 1);
    vTaskDelay(ms / portTICK_PERIOD_MS);
    return 0;
}

// int lua_mcu_(lua_State *L)
// {
//     return 0;
// }

//----------------------------------------------------------------------------------------------------

const struct luaL_Reg LIB_MCU[] = {
    {STR_MCU_MILLIS, lua_mcu_millis},
    {STR_MCU_SLEEP, lua_mcu_sleep},
    {nullptr, nullptr},
};

int lua_register_mcu(lua_State *L)
{
    luaL_newlib(L, LIB_MCU);
    lua_setglobal(L, STR_LIB_NAME_MCU);
    return 0;
}