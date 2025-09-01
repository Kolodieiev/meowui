#pragma GCC optimize("O3")
#include "lua_menu.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "./lua_iwidget_cont.h"
#include "meow/plugin/lua/res/lua_strs.h"

using namespace meow;

int lua_menu_new(lua_State *L)
{
    uint16_t id = luaL_checkinteger(L, 2);
    FixedMenu **ret_wid_ptr = (FixedMenu **)lua_newuserdata(L, sizeof(FixedMenu *));
    *ret_wid_ptr = new FixedMenu(id);
    luaL_getmetatable(L, STR_TYPE_NAME_MENU);
    lua_setmetatable(L, -2);
    return 1;
}

int lua_menu_clone(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    uint16_t id = luaL_checkinteger(L, 2);
    FixedMenu *clone = menu->clone(id);

    FixedMenu **menu_clone = (FixedMenu **)lua_newuserdata(L, sizeof(FixedMenu *));
    *menu_clone = clone;

    luaL_getmetatable(L, STR_TYPE_NAME_MENU);
    lua_setmetatable(L, -2);

    return 1;
}

int lua_menu_focus_up(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    lua_pushboolean(L, menu->focusUp());
    return 1;
}

int lua_menu_focus_down(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    lua_pushboolean(L, menu->focusDown());
    return 1;
}

int lua_menu_delete_widgets(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    menu->delWidgets();
    return 0;
}

int lua_menu_delete_widget_by_id(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    uint16_t id = luaL_checkinteger(L, 2);
    lua_pushboolean(L, menu->delWidgetByID(id));
    return 1;
}

int lua_menu_set_item_h(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    uint16_t h = luaL_checknumber(L, 2);
    menu->setItemHeight(h);
    return 0;
}

int lua_menu_get_item_h(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    lua_pushinteger(L, menu->getItemHeight());
    return 1;
}

int lua_menu_set_item_w(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    uint16_t w = luaL_checknumber(L, 2);
    menu->setItemHeight(w);
    return 0;
}

int lua_menu_get_item_w(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    lua_pushinteger(L, menu->getItemWidth());
    return 1;
}

int lua_menu_set_orientation(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    uint16_t raw_value = luaL_checkinteger(L, 2);
    if (raw_value > IWidget::VERTICAL)
        return luaL_error(L, "Invalid orientation value: %d", raw_value);

    IWidget::Orientation orient = static_cast<IWidget::Orientation>(raw_value);
    menu->setOrientation(orient);
    return 0;
}

int lua_menu_get_curr_item_id(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    lua_pushinteger(L, menu->getCurrItemID());
    return 1;
}

int lua_menu_get_curr_focus_pos(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    lua_pushinteger(L, menu->getCurrFocusPos());
    return 1;
}

int lua_menu_get_curr_item_text(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    lua_pushstring(L, menu->getCurrItemText().c_str());
    return 1;
}

int lua_menu_set_item_spacing(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    uint16_t s = luaL_checknumber(L, 2);
    menu->setItemsSpacing(s);
    return 0;
}

int lua_menu_add_item(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    MenuItem *item = *(MenuItem **)lua_touserdata(L, 2);
    menu->addItem(item);
    return 0;
}

int lua_menu_set_loop_state(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    bool state = lua_toboolean(L, 2);
    menu->setLoopState(state);
    return 0;
}

int lua_menu_set_curr_focus_pos(lua_State *L)
{
    FixedMenu *menu = *(FixedMenu **)lua_touserdata(L, 1);
    uint16_t pos = luaL_checkinteger(L, 2);
    menu->setCurrFocusPos(pos);
    return 0;
}

const struct luaL_Reg TYPE_METH_MENU[] = {
    {"focusUp", lua_menu_focus_up},
    {"focusDown", lua_menu_focus_down},
    {"delWidgets", lua_menu_delete_widgets},
    {"delWidgetByID", lua_menu_delete_widget_by_id},
    {"setItemHeight", lua_menu_set_item_h},
    {"getItemHeight", lua_menu_get_item_h},
    {"setItemWidth", lua_menu_set_item_w},
    {"getItemWidth", lua_menu_get_item_w},
    {"setOrientation", lua_menu_set_orientation},
    {"getCurrItemID", lua_menu_get_curr_item_id},
    {"getCurrFocusPos", lua_menu_get_curr_focus_pos},
    {"getCurrItemText", lua_menu_get_curr_item_text},
    {"setItemsSpacing", lua_menu_set_item_spacing},
    {"addItem", lua_menu_add_item},
    {"setLoopState", lua_menu_set_loop_state},
    {"setCurrFocusPos", lua_menu_set_curr_focus_pos},
    {STR_LUA_WIDGET_CLONE, lua_menu_clone},
    {nullptr, nullptr},
};

void lua_init_menu(lua_State *L)
{
    luaL_newmetatable(L, STR_TYPE_NAME_MENU);
    lua_newtable(L);
    luaL_setfuncs(L, TYPE_METH_MENU, 0);
    luaL_getmetatable(L, STR_TYPE_NAME_IWIDGET_CONT);
    lua_setmetatable(L, -2);
    lua_setfield(L, -2, STR_LUA_INDEX);
    lua_pop(L, 1);

    lua_newtable(L);
    lua_pushcfunction(L, lua_menu_new);
    lua_setfield(L, -2, STR_LUA_NEW);
    lua_setglobal(L, STR_TYPE_NAME_MENU);
}