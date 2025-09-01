#pragma GCC optimize("O3")
#pragma once
#include "../../lua.h"

const char STR_LIB_NAME_UART[] = "uart";

int lua_open_uart(lua_State *L);