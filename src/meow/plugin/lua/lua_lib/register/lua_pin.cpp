#include "lua_pin.h"
#include <Arduino.h>

const char STR_LIB_NAME_PIN[] = "pin";
//

int lua_pin_input(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    pinMode(pin, INPUT);
    return 0;
}

int lua_pin_output(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    pinMode(pin, OUTPUT);
    return 0;
}

int lua_pin_pullup(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    pinMode(pin, INPUT_PULLUP);
    return 0;
}

int lua_pin_toggle(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    digitalWrite(pin, !digitalRead(pin));
    return 0;
}

int lua_pin_write(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    int value = luaL_checkinteger(L, 2);
    digitalWrite(pin, value);
    return 0;
}

int lua_pin_read(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    lua_pushinteger(L, digitalRead(pin));
    return 1;
}

int lua_pin_aread(lua_State *L)
{
    int pin = luaL_checkinteger(L, 1);
    lua_pushinteger(L, analogRead(pin));
    return 1;
}

//----------------------------------------------------------------------------------------------------

const struct luaL_Reg LIB_PIN[] = {
    {"input", lua_pin_input},
    {"output", lua_pin_output},
    {"pullup", lua_pin_pullup},
    {"toggle", lua_pin_toggle},
    {"read", lua_pin_read},
    {"analogRead", lua_pin_aread},
    {nullptr, nullptr},
};

int lua_register_pin(lua_State *L)
{
    luaL_newlib(L, LIB_PIN);
    lua_setglobal(L, STR_LIB_NAME_PIN);
    return 0;
}