#include "lua_pwm.h"
#include <Arduino.h>
#include "meow/plugin/lua/lua_lib/helper/lua_helper.h"

//
const char STR_PMW_SETUP[] = "setup";
const char STR_PMW_STOP[] = "stop";
const char STR_PMW_ATTACH[] = "attach";
const char STR_PWM_DETACH[] = "detach";
const char STR_PWM_SET[] = "set";

int lua_pwm_setup(lua_State *L)
{
    if (lua_check_top(L, {3}) == 0)
        return 0;

    int channel = luaL_checkinteger(L, 1);
    int frequency = luaL_checkinteger(L, 2);
    int resolution = luaL_checkinteger(L, 3);

    ledcSetup(channel, frequency, resolution);
    return 0;
}

int lua_pwm_stop(lua_State *L)
{
    int channel = luaL_checkinteger(L, 1);
    ledcWrite(channel, 0);
    return 0;
}

int lua_pwm_attach(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    int channel = luaL_checkinteger(L, 2);
    ledcAttachPin(pin, channel);
    return 0;
}

int lua_pwm_detach(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    ledcDetachPin(pin);
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
    {STR_PMW_SETUP, lua_pwm_setup},
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