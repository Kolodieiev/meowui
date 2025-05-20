#include "custom_type_initializer.h"
#include <vector>
#include <unordered_map>

#include "./type/widget/lua_iwidget.h"
#include "./type/widget/lua_iwidget_cont.h"
#include "./type/widget/lua_empty_layout.h"
#include "./type/widget/lua_label.h"
#include "./type/widget/lua_image.h"
#include "./type/widget/lua_menu.h"
#include "./type/widget/lua_menu_item.h"
#include "./type/widget/lua_toggle_item.h"
#include "./type/widget/lua_toggle_switch.h"
#include "./type/widget/lua_progress.h"

typedef std::function<void(lua_State *L)> InitLuaTypeFunc;

typedef struct LuaTypeInit
{
    const char *name;
    InitLuaTypeFunc func;

    LuaTypeInit(const char *n, InitLuaTypeFunc f) : name(n), func(f) {}
} LuaTypeInit;

bool _warmed_up{false};
std::vector<const char *> _inited_types;
std::vector<LuaTypeInit> _available_types;

bool is_type_inited(const char *type_name)
{
    for (auto it_b = _inited_types.begin(), it_e = _inited_types.end(); it_b != it_e; ++it_b)
    {
        if (strcmp(*it_b, type_name) == 0)
            return true;
    }

    return false;
}

InitLuaTypeFunc get_type_initializer(const char *type_name)
{
    for (auto it_b = _available_types.begin(), it_e = _available_types.end(); it_b != it_e; ++it_b)
    {
        if (strcmp(it_b->name, type_name) == 0)
            return it_b->func;
    }

    return nullptr;
}

void init_type(lua_State *L, const char *type_name)
{
    if (is_type_inited(type_name))
        return;

    auto func = get_type_initializer(type_name);
    if (!func)
    {
        luaL_error(L, "Відсутній ініціалізатор для типу: %s", type_name);
        return;
    }

    _inited_types.push_back(type_name);
    func(L);
}

void init_iwidget(lua_State *L)
{
    lua_init_iwidget(L);
}

void init_iwidget_cont(lua_State *L)
{
    init_type(L, STR_TYPE_NAME_IWIDGET);
    lua_init_iwidget_cont(L);
}

void init_label(lua_State *L)
{
    init_type(L, STR_TYPE_NAME_IWIDGET);
    lua_init_label(L);
}

void init_image(lua_State *L)
{
    init_type(L, STR_TYPE_NAME_IWIDGET);
    lua_init_image(L);
}

void init_empty_layout(lua_State *L)
{
    init_type(L, STR_TYPE_NAME_IWIDGET_CONT);
    lua_init_empty_layout(L);
}

void init_menu(lua_State *L)
{
    init_type(L, STR_TYPE_NAME_IWIDGET_CONT);
    lua_init_menu(L);
}

void init_menu_item(lua_State *L)
{
    init_type(L, STR_TYPE_NAME_LABEL);
    init_type(L, STR_TYPE_NAME_IMAGE);
    lua_init_menu_item(L);
}

void init_toggle_item(lua_State *L)
{
    init_type(L, STR_TYPE_NAME_MENU_ITEM);
    init_type(L, STR_TYPE_NAME_TOGGLE_SWITCH);
    lua_init_toggle_item(L);
}

void init_toggle_switch(lua_State *L)
{
    init_type(L, STR_TYPE_NAME_IWIDGET);
    lua_init_toggle_switch(L);
}

void init_progress(lua_State *L)
{
    init_type(L, STR_TYPE_NAME_IWIDGET);
    lua_init_progress(L);
}

void warm_up()
{
    _available_types.emplace_back(STR_TYPE_NAME_IWIDGET, init_iwidget);
    _available_types.emplace_back(STR_TYPE_NAME_IWIDGET_CONT, init_iwidget_cont);
    _available_types.emplace_back(STR_TYPE_NAME_LABEL, init_label);
    _available_types.emplace_back(STR_TYPE_NAME_IMAGE, init_image);
    _available_types.emplace_back(STR_TYPE_NAME_EMPTY_LAYOUT, init_empty_layout);
    _available_types.emplace_back(STR_TYPE_NAME_MENU, init_menu);
    _available_types.emplace_back(STR_TYPE_NAME_MENU_ITEM, init_menu_item);
    _available_types.emplace_back(STR_TYPE_NAME_TOGGLE_ITEM, init_toggle_item);
    _available_types.emplace_back(STR_TYPE_NAME_TOGGLE_SWITCH, init_toggle_switch);
    _available_types.emplace_back(STR_TYPE_NAME_PROGRESS, init_progress);

    _warmed_up = true;
}

void init_custom_type(lua_State *L)
{
    if (!_warmed_up)
        warm_up();

    const char *type_name = luaL_checkstring(L, 1);
    init_type(L, type_name);
}

void reset_custom_initializer()
{
    _warmed_up = false;
    _inited_types.clear();
    _available_types.clear();
}
