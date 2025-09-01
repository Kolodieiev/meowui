#pragma GCC optimize("O3")
#pragma once
#include "../../lua.h"

const char STR_LIB_NAME_I2C[] = "i2c";

int lua_open_i2c(lua_State *L);