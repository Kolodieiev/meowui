#pragma GCC optimize("O3")
#include "lua_progress.h"
#include "meow/ui/widget/progress/ProgressBar.h"
#include "./lua_iwidget.h"
#include "meow/plugin/lua/res/lua_strs.h"

using namespace meow;

int lua_progress_new(lua_State *L)
{
    uint16_t id = luaL_checkinteger(L, 2);
    ProgressBar **ret_wid_ptr = (ProgressBar **)lua_newuserdata(L, sizeof(ProgressBar *));
    *ret_wid_ptr = new ProgressBar(id);
    luaL_getmetatable(L, STR_TYPE_NAME_PROGRESS);
    lua_setmetatable(L, -2);

    return 1;
}

int lua_progress_clone(lua_State *L)
{
    ProgressBar *progress = *(ProgressBar **)lua_touserdata(L, 1);
    uint16_t id = luaL_checkinteger(L, 2);
    ProgressBar *clone = progress->clone(id);

    ProgressBar **progress_clone = (ProgressBar **)lua_newuserdata(L, sizeof(ProgressBar *));
    *progress_clone = clone;

    luaL_getmetatable(L, STR_TYPE_NAME_PROGRESS);
    lua_setmetatable(L, -2);

    return 1;
}

int lua_progress_set_max(lua_State *L)
{
    ProgressBar *progress = *(ProgressBar **)lua_touserdata(L, 1);
    uint16_t max = luaL_checkinteger(L, 2);
    progress->setMax(max);
    return 0;
}

int lua_progress_get_max(lua_State *L)
{
    ProgressBar *progress = *(ProgressBar **)lua_touserdata(L, 1);
    lua_pushinteger(L, progress->getMax());
    return 1;
}

int lua_progress_set_progress(lua_State *L)
{
    ProgressBar *progress = *(ProgressBar **)lua_touserdata(L, 1);
    uint16_t pr_val = luaL_checkinteger(L, 2);
    progress->setProgress(pr_val);
    return 0;
}

int lua_progress_get_progress(lua_State *L)
{
    ProgressBar *progress = *(ProgressBar **)lua_touserdata(L, 1);
    lua_pushinteger(L, progress->getProgress());
    return 1;
}

int lua_progress_set_progress_color(lua_State *L)
{
    ProgressBar *progress = *(ProgressBar **)lua_touserdata(L, 1);
    uint16_t color = luaL_checkinteger(L, 2);
    progress->setProgressColor(color);
    return 0;
}

int lua_progress_set_orientation(lua_State *L)
{
    ProgressBar *progress = *(ProgressBar **)lua_touserdata(L, 1);
    uint16_t raw_value = luaL_checkinteger(L, 2);
    if (raw_value > IWidget::VERTICAL)
        return luaL_error(L, "Invalid orientation value: %d", raw_value);

    IWidget::Orientation orient = static_cast<IWidget::Orientation>(raw_value);
    progress->setOrientation(orient);
    return 0;
}

int lua_progress_reset(lua_State *L)
{
    ProgressBar *progress = *(ProgressBar **)lua_touserdata(L, 1);
    progress->reset();
    return 0;
}

const struct luaL_Reg TYPE_METH_PROGRESS[] = {
    {"setMax", lua_progress_set_max},
    {"getMax", lua_progress_get_max},
    {"setProgress", lua_progress_set_progress},
    {"getProgress", lua_progress_get_progress},
    {"setProgressColor", lua_progress_set_progress_color},
    {"setOrientation", lua_progress_set_orientation},
    {"reset", lua_progress_reset},
    {STR_LUA_WIDGET_CLONE, lua_progress_clone},
    {nullptr, nullptr},
};

void lua_init_progress(lua_State *L)
{
    luaL_newmetatable(L, STR_TYPE_NAME_PROGRESS);
    lua_newtable(L);
    luaL_setfuncs(L, TYPE_METH_PROGRESS, 0);
    luaL_getmetatable(L, STR_TYPE_NAME_IWIDGET);
    lua_setmetatable(L, -2);
    lua_setfield(L, -2, STR_LUA_INDEX);
    lua_pop(L, 1);

    lua_newtable(L);
    lua_pushcfunction(L, lua_progress_new);
    lua_setfield(L, -2, STR_LUA_NEW);
    lua_setglobal(L, STR_TYPE_NAME_PROGRESS);
}