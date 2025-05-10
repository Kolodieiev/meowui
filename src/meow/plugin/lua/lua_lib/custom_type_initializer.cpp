#include "custom_type_initializer.h"
#include "./type/widget/lua_iwidget.h"
#include "./type/widget/lua_iwidget_cont.h"
#include "./type/widget/lua_empty_layout.h"
#include "./type/widget/lua_label.h"

const char STR_ERR_NO_INITIALIZER[] = "Відсутній ініціалізатор для типу: %s";

void init_custom_type(lua_State *L)
{
    const char *type_name = luaL_checkstring(L, 1);
    if (!strcmp(type_name, STR_TYPE_NAME_IWIDGET))
        lua_init_iwidget(L);
    else if (strcmp(type_name, STR_TYPE_NAME_IWIDGET_CONT) == 0)
        lua_init_iwidget_cont(L);
    else if (strcmp(type_name, STR_TYPE_NAME_LABEL) == 0)
        lua_init_label(L);
    else if (strcmp(type_name, STR_TYPE_NAME_EMPTY_LAYOUT) == 0)
        lua_init_empty_layout(L);
    else
        luaL_error(L, STR_ERR_NO_INITIALIZER, type_name);
}

void deinit_custom_type(lua_State *L)
{
    const char *type_name = luaL_checkstring(L, 1);
    if (!strcmp(type_name, STR_TYPE_NAME_IWIDGET))
        lua_deinit_iwidget(L);
    else if (strcmp(type_name, STR_TYPE_NAME_IWIDGET_CONT) == 0)
        lua_deinit_iwidget_cont(L);
    else if (strcmp(type_name, STR_TYPE_NAME_LABEL) == 0)
        lua_deinit_label(L);
    else if (strcmp(type_name, STR_TYPE_NAME_EMPTY_LAYOUT) == 0)
        lua_deinit_empty_layout(L);
    else
        luaL_error(L, STR_ERR_NO_INITIALIZER, type_name);
}