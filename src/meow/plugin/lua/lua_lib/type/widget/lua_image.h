#pragma once
#include "../../../lua.h"

const char STR_TYPE_NAME_IMAGE[] = "Image";

void lua_init_image(lua_State *L);
void lua_deinit_image(lua_State *L);