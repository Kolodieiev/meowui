#include "lua_label.h"
#include "meow/ui/widget/text/Label.h"
#include "./lua_iwidget.h"
#include "meow/plugin/lua/res/lua_strs.h"
#include "meow/plugin/lua/res/lua_err_msg.h"

using namespace meow;

int lua_label_new(lua_State *L)
{
    uint16_t id = luaL_checkinteger(L, 1);
    Label **r_widget = (Label **)lua_newuserdata(L, sizeof(Label *));
    *r_widget = new Label(id);
    return 1;
}

int lua_label_clone(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    uint16_t id = luaL_checkinteger(L, 2);
    Label *clone = label->clone(id);

    Label **label_clone = (Label **)lua_newuserdata(L, sizeof(Label *));
    *label_clone = clone;

    luaL_getmetatable(L, STR_TYPE_NAME_LABEL);
    lua_setmetatable(L, -2);

    return 1;
}

int lua_label_init_width_to_fit(lua_State *L)
{
    return 0;
}

int lua_label_update_width_to_fit(lua_State *L)
{
    return 0;
}

int lua_label_set_text(lua_State *L)
{
    return 0;
}

int lua_label_get_text(lua_State *L)
{
    return 0;
}

int lua_label_set_text_size(lua_State *L)
{
    return 0;
}

int lua_label_set_text_color(lua_State *L)
{
    return 0;
}

int lua_label_set_font_id(lua_State *L)
{
    return 0;
}

int lua_label_set_gravity(lua_State *L)
{
    return 0;
}

int lua_label_set_align(lua_State *L)
{
    return 0;
}

int lua_label_set_text_offset(lua_State *L)
{
    return 0;
}

int lua_label_get_text_len(lua_State *L)
{
    return 0;
}

int lua_label_(lua_State *L)
{
    return 0;
}

int lua_label_set_ticker(lua_State *L)
{
    return 0;
}

int lua_label_set_ticker_in_focus(lua_State *L)
{
    return 0;
}

int lua_label_set_back_img(lua_State *L)
{
    return 0;
}

int lua_label_set_multiline(lua_State *L)
{
    return 0;
}

int lua_label_set_ticker_delay(lua_State *L)
{
    return 0;
}

const struct luaL_Reg TYPE_METH_LABEL[] = {
    {"initWidthToFit", lua_label_init_width_to_fit},
    {"updateWidthToFit", lua_label_update_width_to_fit},
    {"setText", lua_label_set_text},
    {"getText", lua_label_get_text},
    {"setTextSize", lua_label_set_text_size},
    {"setTextColor", lua_label_set_text_color},
    {"setFontID", lua_label_set_font_id},
    {"setGravity", lua_label_set_gravity},
    {"setAlign", lua_label_set_align},
    {"setTextOffset", lua_label_set_text_offset},
    {"getLen", lua_label_get_text_len},
    {"setTicker", lua_label_set_ticker},
    {"setTickerInFocus", lua_label_set_ticker_in_focus},
    {"setBackImg", lua_label_set_back_img},
    {"setMultiline", lua_label_set_multiline},
    {"setTickerDelay", lua_label_set_ticker_delay},
    {STR_LUA_NEW, lua_label_new},
    {STR_LUA_WIDGET_CLONE, lua_label_clone},
    {nullptr, nullptr},
};

void lua_init_label(lua_State *L)
{
    lua_init_iwidget(L, STR_TYPE_NAME_LABEL);

    luaL_newmetatable(L, STR_TYPE_NAME_LABEL);
    luaL_setfuncs(L, TYPE_METH_LABEL, 0);
    luaL_getmetatable(L, STR_TYPE_NAME_IWIDGET);
    lua_setfield(L, -2, STR_LUA_INDEX);
    lua_pop(L, 1);
}

void lua_deinit_label(lua_State *L)
{
    lua_pushnil(L);
    lua_setfield(L, LUA_REGISTRYINDEX, STR_TYPE_NAME_LABEL);
    lua_deinit_iwidget(L, STR_TYPE_NAME_LABEL);
}
