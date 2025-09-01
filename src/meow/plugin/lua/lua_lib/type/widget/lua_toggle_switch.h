#pragma once
#pragma GCC optimize("O3")
#include "meow/plugin/lua/lua.h"

const char STR_TYPE_NAME_TOGGLE_SWITCH[] = "ToggleSwitch";

void lua_init_toggle_switch(lua_State *L);
