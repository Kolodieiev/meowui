#include "lua_empty_layout.h"
#include "meow/ui/widget/layout/EmptyLayout.h"
#include "./lua_iwidget_cont.h"
#include "meow/plugin/lua/res/lua_strs.h"
#include "meow/plugin/lua/res/lua_err_msg.h"

using namespace meow;

int lua_el_new(lua_State *L)
{
    uint16_t id = luaL_checkinteger(L, 2);
    EmptyLayout **ret_wid_ptr = (EmptyLayout **)lua_newuserdata(L, sizeof(EmptyLayout *));
    *ret_wid_ptr = new EmptyLayout(id);
    luaL_getmetatable(L, STR_TYPE_NAME_EMPTY_LAYOUT);
    lua_setmetatable(L, -2);
    return 1;
}

int lua_el_clone(lua_State *L)
{
    EmptyLayout *el = *(EmptyLayout **)lua_touserdata(L, 1);
    uint16_t id = luaL_checkinteger(L, 2);
    EmptyLayout *clone = el->clone(id);

    EmptyLayout **el_clone = (EmptyLayout **)lua_newuserdata(L, sizeof(EmptyLayout *));
    *el_clone = clone;

    luaL_getmetatable(L, STR_TYPE_NAME_EMPTY_LAYOUT);
    lua_setmetatable(L, -2);

    return 1;
}

const struct luaL_Reg TYPE_METH_EMPTY_LAYOUT[] = {
    {STR_LUA_WIDGET_CLONE, lua_el_clone},
    {nullptr, nullptr},
};

void lua_init_empty_layout(lua_State *L)
{
    lua_init_iwidget_cont(L, STR_TYPE_NAME_IWIDGET_CONT);

    luaL_newmetatable(L, STR_TYPE_NAME_EMPTY_LAYOUT);
    lua_newtable(L);
    luaL_setfuncs(L, TYPE_METH_EMPTY_LAYOUT, 0);
    luaL_getmetatable(L, STR_TYPE_NAME_IWIDGET_CONT);
    lua_setmetatable(L, -2);
    lua_setfield(L, -2, STR_LUA_INDEX);
    lua_pop(L, 1);

    lua_newtable(L);
    lua_pushcfunction(L, lua_el_new);
    lua_setfield(L, -2, STR_LUA_NEW);
    lua_setglobal(L, STR_TYPE_NAME_EMPTY_LAYOUT);
}

void lua_deinit_empty_layout(lua_State *L)
{
    lua_pushnil(L);
    lua_setfield(L, LUA_REGISTRYINDEX, STR_TYPE_NAME_EMPTY_LAYOUT);
    lua_deinit_iwidget_cont(L, STR_TYPE_NAME_EMPTY_LAYOUT);
    lua_pushnil(L);
    lua_setglobal(L, STR_TYPE_NAME_EMPTY_LAYOUT);
}
