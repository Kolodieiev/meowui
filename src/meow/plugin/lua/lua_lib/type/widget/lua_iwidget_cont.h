#pragma once
#include "../../../lua.h"

const char STR_TYPE_NAME_IWIDGET_CONT[] = "IWidgetContainer";

void lua_init_iwidget_cont(lua_State *L, const char *type_caller_name = emptyString.c_str());
void lua_deinit_iwidget_cont(lua_State *L, const char *type_caller_name = emptyString.c_str());
void lua_clear_iwidget_cont();