#include "lua_label.h"
#include "meow/ui/widget/text/Label.h"
#include "meow/ui/widget/image/Image.h"
#include "./lua_iwidget.h"
#include "meow/plugin/lua/res/lua_strs.h"

using namespace meow;

int lua_label_new(lua_State *L)
{
    uint16_t id = luaL_checkinteger(L, 2);
    Label **ret_wid_ptr = (Label **)lua_newuserdata(L, sizeof(Label *));
    *ret_wid_ptr = new Label(id);
    luaL_getmetatable(L, STR_TYPE_NAME_LABEL);
    lua_setmetatable(L, -2);

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
    Label *label = *(Label **)lua_touserdata(L, 1);
    uint16_t add_val = luaL_checknumber(L, 2);
    label->initWidthToFit(add_val);
    return 0;
}

int lua_label_update_width_to_fit(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    uint16_t add_val = luaL_checknumber(L, 2);
    label->updateWidthToFit(add_val);
    return 0;
}

int lua_label_set_text(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    const char *text = luaL_checkstring(L, 2);
    label->setText(text);
    return 0;
}

int lua_label_get_text(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    lua_pushstring(L, label->getText().c_str());
    return 1;
}

int lua_label_set_text_size(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    uint8_t size = luaL_checkinteger(L, 2);
    label->setTextSize(size);
    return 0;
}

int lua_label_set_text_color(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    uint16_t color = luaL_checkinteger(L, 2);
    label->setTextColor(color);
    return 0;
}

int lua_label_set_font_id(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    uint8_t font_id = luaL_checkinteger(L, 2);
    label->setFontID(font_id);
    return 0;
}

int lua_label_set_gravity(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    uint16_t raw_value = luaL_checkinteger(L, 2);
    if (raw_value > IWidget::GRAVITY_BOTTOM)
        return luaL_error(L, "Invalid gravity value: %d", raw_value);

    IWidget::Gravity gravity = static_cast<IWidget::Gravity>(raw_value);
    label->setGravity(gravity);
    return 0;
}

int lua_label_set_align(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    uint16_t raw_value = luaL_checkinteger(L, 2);
    if (raw_value > IWidget::ALIGN_END)
        return luaL_error(L, "Invalid alignment value: %d", raw_value);

    IWidget::Alignment align = static_cast<IWidget::Alignment>(raw_value);
    label->setAlign(align);
    return 0;
}

int lua_label_set_text_offset(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    uint8_t offset = luaL_checkinteger(L, 2);
    label->setTextOffset(offset);
    return 0;
}

int lua_label_get_text_len(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    lua_pushinteger(L, label->getLen());
    return 1;
}

int lua_label_set_ticker(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    bool state = lua_toboolean(L, 2);
    label->setTicker(state);
    return 0;
}

int lua_label_set_ticker_in_focus(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    bool state = lua_toboolean(L, 2);
    label->setTickerInFocus(state);
    return 0;
}

int lua_label_set_back_img(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    Image **image = (Image **)lua_touserdata(L, 2);
    label->setBackImg(*image);

    return 0;
}

int lua_label_set_multiline(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    bool state = lua_toboolean(L, 2);
    label->setMultiline(state);
    return 0;
}

int lua_label_set_ticker_delay(lua_State *L)
{
    Label *label = *(Label **)lua_touserdata(L, 1);
    int delay = luaL_checkinteger(L, 2);
    label->setTickerDelay(delay);
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
    {STR_LUA_WIDGET_CLONE, lua_label_clone},
    {nullptr, nullptr},
};

void lua_init_label(lua_State *L)
{
    luaL_newmetatable(L, STR_TYPE_NAME_LABEL);
    lua_newtable(L);
    luaL_setfuncs(L, TYPE_METH_LABEL, 0);
    luaL_getmetatable(L, STR_TYPE_NAME_IWIDGET);
    lua_setmetatable(L, -2);
    lua_setfield(L, -2, STR_LUA_INDEX);
    lua_pop(L, 1);

    lua_newtable(L);
    lua_pushcfunction(L, lua_label_new);
    lua_setfield(L, -2, STR_LUA_NEW);
    lua_setglobal(L, STR_TYPE_NAME_LABEL);
}
