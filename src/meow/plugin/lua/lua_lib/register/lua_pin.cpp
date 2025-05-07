#include "lua_pin.h"
#include <Arduino.h>

const char STR_LIB_NAME_PIN[] = "pin";
//
const char STR_PIN_INPUT[] = "input";
const char STR_PIN_OUTPUT[] = "output";
const char STR_PIN_PULLUP[] = "pullup";
const char STR_PIN_TOGGLE[] = "toggle";
const char STR_PIN_WRITE[] = "write";
const char STR_PIN_READ[] = "read";
const char STR_PIN_AREAD[] = "analogRead";

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
    {STR_PIN_INPUT, lua_pin_input},
    {STR_PIN_OUTPUT, lua_pin_output},
    {STR_PIN_PULLUP, lua_pin_pullup},
    {STR_PIN_TOGGLE, lua_pin_toggle},
    {STR_PIN_WRITE, lua_pin_write},
    {STR_PIN_READ, lua_pin_read},
    {STR_PIN_AREAD, lua_pin_aread},
    {nullptr, nullptr},
};

int lua_register_pin(lua_State *L)
{
    luaL_newlib(L, LIB_PIN);
    lua_setglobal(L, STR_LIB_NAME_PIN);
    return 0;
}