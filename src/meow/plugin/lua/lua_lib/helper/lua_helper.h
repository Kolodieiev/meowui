#pragma once
#include "../../lua.h"

uint8_t lua_check_top(lua_State* L, std::initializer_list<uint8_t> args_assert);