#include "lua_toggle_switch.h"
#include "./lua_iwidget.h"
#include "meow/plugin/lua/res/lua_strs.h"
#include "meow/ui/widget/toggle/ToggleSwitch.h"

using namespace meow;

int lua_toggle_switch_new(lua_State *L)
{
    uint16_t id = luaL_checkinteger(L, 2);
    ToggleSwitch **ret_wid_ptr = (ToggleSwitch **)lua_newuserdata(L, sizeof(ToggleSwitch *));
    *ret_wid_ptr = new ToggleSwitch(id);
    luaL_getmetatable(L, STR_TYPE_NAME_TOGGLE_SWITCH);
    lua_setmetatable(L, -2);
    return 1;
}

int lua_toggle_switch_clone(lua_State *L)
{
    ToggleSwitch *toggle_switch = *(ToggleSwitch **)lua_touserdata(L, 1);
    uint16_t id = luaL_checkinteger(L, 2);
    ToggleSwitch *clone = toggle_switch->clone(id);

    ToggleSwitch **toggle_switch_clone = (ToggleSwitch **)lua_newuserdata(L, sizeof(ToggleSwitch *));
    *toggle_switch_clone = clone;

    luaL_getmetatable(L, STR_TYPE_NAME_TOGGLE_SWITCH);
    lua_setmetatable(L, -2);
    return 1;
}

int lua_toggle_switch_is_on(lua_State *L)
{
    ToggleSwitch *toggle_switch = *(ToggleSwitch **)lua_touserdata(L, 1);
    lua_pushboolean(L, toggle_switch->isOn());
    return 1;
}

int lua_toggle_switch_on(lua_State *L)
{
    ToggleSwitch *toggle_switch = *(ToggleSwitch **)lua_touserdata(L, 1);
    toggle_switch->on();
    return 0;
}

int lua_toggle_switch_off(lua_State *L)
{
    ToggleSwitch *toggle_switch = *(ToggleSwitch **)lua_touserdata(L, 1);
    toggle_switch->off();
    return 0;
}

int lua_toggle_switch_set_lever_color(lua_State *L)
{
    ToggleSwitch *toggle_switch = *(ToggleSwitch **)lua_touserdata(L, 1);
    uint16_t color = luaL_checkinteger(L, 2);
    toggle_switch->setLeverColor(color);
    return 0;
}

int lua_toggle_switch_set_on_color(lua_State *L)
{
    ToggleSwitch *toggle_switch = *(ToggleSwitch **)lua_touserdata(L, 1);
    uint16_t color = luaL_checkinteger(L, 2);
    toggle_switch->setOnColor(color);
    return 0;
}

int lua_toggle_switch_set_off_color(lua_State *L)
{
    ToggleSwitch *toggle_switch = *(ToggleSwitch **)lua_touserdata(L, 1);
    uint16_t color = luaL_checkinteger(L, 2);
    toggle_switch->setOffColor(color);
    return 0;
}

int lua_toggle_switch_set_orientation(lua_State *L)
{
    ToggleSwitch *toggle_switch = *(ToggleSwitch **)lua_touserdata(L, 1);
    uint16_t raw_value = luaL_checkinteger(L, 2);
    if (raw_value > IWidget::VERTICAL)
        return luaL_error(L, "Invalid orientation value: %d", raw_value);

    IWidget::Orientation orient = static_cast<IWidget::Orientation>(raw_value);
    toggle_switch->setOrientation(orient);
    return 0;
}

const struct luaL_Reg TYPE_METH_TOGGLE_SWITCH[] = {
    {"isOn", lua_toggle_switch_is_on},
    {"on", lua_toggle_switch_on},
    {"off", lua_toggle_switch_off},
    {"setLeverColor", lua_toggle_switch_set_lever_color},
    {"setOnColor", lua_toggle_switch_set_on_color},
    {"setOffColor", lua_toggle_switch_set_off_color},
    {"setOrientation", lua_toggle_switch_set_orientation},
    {STR_LUA_WIDGET_CLONE, lua_toggle_switch_clone},
    {nullptr, nullptr},
};

void lua_init_toggle_switch(lua_State *L)
{
    luaL_newmetatable(L, STR_TYPE_NAME_TOGGLE_SWITCH);
    lua_newtable(L);
    luaL_setfuncs(L, TYPE_METH_TOGGLE_SWITCH, 0);
    luaL_getmetatable(L, STR_TYPE_NAME_IWIDGET);
    lua_setmetatable(L, -2);
    lua_setfield(L, -2, STR_LUA_INDEX);
    lua_pop(L, 1);

    lua_newtable(L);
    lua_pushcfunction(L, lua_toggle_switch_new);
    lua_setfield(L, -2, STR_LUA_NEW);
    lua_setglobal(L, STR_TYPE_NAME_TOGGLE_SWITCH);
}
