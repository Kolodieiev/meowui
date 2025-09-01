#pragma GCC optimize("O3")
#include "lua_pwm.h"
#include <Arduino.h>
#include "esp32-hal-ledc.h"
#include "meow/plugin/lua/lua_lib/helper/lua_helper.h"

//
const char STR_PMW_STOP[] = "stop";
const char STR_PMW_ATTACH[] = "attach";
const char STR_PWM_DETACH[] = "detach";
const char STR_PWM_SET[] = "set";

int lua_pwm_stop(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    ledcWrite(pin, 0);
    return 0;
}

int lua_pwm_attach(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    int freq = luaL_checkinteger(L, 2);
    int res = luaL_checkinteger(L, 3);
    ledcAttach(pin, freq, res);
    return 0;
}

int lua_pwm_detach(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    ledcDetach(pin);
    return 0;
}

int lua_pwm_set(lua_State *L)
{
    int channel = luaL_checkinteger(L, 1);
    int value = luaL_checkinteger(L, 2);
    ledcWrite(channel, value);
    return 0;
}

//----------------------------------------------------------------------------------------------------

const struct luaL_Reg LIB_PWM[] = {
    {STR_PMW_STOP, lua_pwm_stop},
    {STR_PMW_ATTACH, lua_pwm_attach},
    {STR_PWM_DETACH, lua_pwm_detach},
    {STR_PWM_SET, lua_pwm_set},
    {nullptr, nullptr},
};

int lua_open_pwm(lua_State *L)
{
    luaL_newlib(L, LIB_PWM);
    return 1;
}