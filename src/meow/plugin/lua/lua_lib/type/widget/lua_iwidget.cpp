#include "lua_iwidget.h"
#include "meow/ui/widget/IWidget.h"
#include "meow/plugin/lua/res/lua_strs.h"

using namespace meow;
//
const char STR_ALIGN_START[] = "ALIGN_START";
const char STR_ALIGN_CENTER[] = "ALIGN_CENTER";
const char STR_ALIGN_END[] = "ALIGN_END";
//
const char STR_GRAVITY_TOP[] = "GRAVITY_TOP";
const char STR_GRAVITY_CENTER[] = "GRAVITY_CENTER";
const char STR_GRAVITY_BOTTOM[] = "GRAVITY_BOTTOM";
//
const char STR_ORIENTATION_HORIZONTAL[] = "HORIZONTAL";
const char STR_ORIENTATION_VERTICAL[] = "VERTICAL";
//
const char STR_VISIBILITY_VISIBLE[] = "VISIBLE";
const char STR_VISIBILITY_INVISIBLE[] = "INVISIBLE";

void init_iwidget_constants(lua_State *L)
{
    // Alignment
    lua_pushinteger(L, IWidget::ALIGN_START);
    lua_setglobal(L, STR_ALIGN_START);
    lua_pushinteger(L, IWidget::ALIGN_CENTER);
    lua_setglobal(L, STR_ALIGN_CENTER);
    lua_pushinteger(L, IWidget::ALIGN_END);
    lua_setglobal(L, STR_ALIGN_END);
    // Gravity
    lua_pushinteger(L, IWidget::GRAVITY_TOP);
    lua_setglobal(L, STR_GRAVITY_TOP);
    lua_pushinteger(L, IWidget::GRAVITY_CENTER);
    lua_setglobal(L, STR_GRAVITY_CENTER);
    lua_pushinteger(L, IWidget::GRAVITY_BOTTOM);
    lua_setglobal(L, STR_GRAVITY_BOTTOM);
    // Orientation
    lua_pushinteger(L, IWidget::HORIZONTAL);
    lua_setglobal(L, STR_ORIENTATION_HORIZONTAL);
    lua_pushinteger(L, IWidget::VERTICAL);
    lua_setglobal(L, STR_ORIENTATION_VERTICAL);
    // Visibility
    lua_pushinteger(L, IWidget::VISIBLE);
    lua_setglobal(L, STR_VISIBILITY_VISIBLE);
    lua_pushinteger(L, IWidget::INVISIBLE);
    lua_setglobal(L, STR_VISIBILITY_INVISIBLE);
}

void deinit_iwidget_constants(lua_State *L)
{
    // Alignment
    lua_pushnil(L);
    lua_setglobal(L, STR_ALIGN_START);
    lua_pushnil(L);
    lua_setglobal(L, STR_ALIGN_CENTER);
    lua_pushnil(L);
    lua_setglobal(L, STR_ALIGN_END);
    // Gravity
    lua_pushnil(L);
    lua_setglobal(L, STR_GRAVITY_TOP);
    lua_pushnil(L);
    lua_setglobal(L, STR_GRAVITY_CENTER);
    lua_pushnil(L);
    lua_setglobal(L, STR_GRAVITY_BOTTOM);
    // Orientation
    lua_pushnil(L);
    lua_setglobal(L, STR_ORIENTATION_HORIZONTAL);
    lua_pushnil(L);
    lua_setglobal(L, STR_ORIENTATION_VERTICAL);
    // Visibility
    lua_pushnil(L);
    lua_setglobal(L, STR_VISIBILITY_VISIBLE);
    lua_pushnil(L);
    lua_setglobal(L, STR_VISIBILITY_INVISIBLE);
}

const char STR_COLOR_TRANSPARENT[] = "TFT_TRANSPARENT";
const char STR_COLOR_BLACK[] = "TFT_BLACK";
const char STR_COLOR_WHITE[] = "TFT_WHITE";
const char STR_COLOR_RED[] = "TFT_RED";
const char STR_COLOR_GREEN[] = "TFT_GREEN";
const char STR_COLOR_BLUE[] = "TFT_BLUE";
const char STR_COLOR_YELLOW[] = "TFT_YELLOW";
const char STR_COLOR_ORANGE[] = "TFT_ORANGE";

void init_color_constants(lua_State *L)
{
    lua_pushinteger(L, TFT_TRANSPARENT);
    lua_setglobal(L, STR_COLOR_TRANSPARENT);

    lua_pushinteger(L, TFT_BLACK);
    lua_setglobal(L, STR_COLOR_BLACK);

    lua_pushinteger(L, TFT_WHITE);
    lua_setglobal(L, STR_COLOR_WHITE);

    lua_pushinteger(L, TFT_RED);
    lua_setglobal(L, STR_COLOR_RED);

    lua_pushinteger(L, TFT_GREEN);
    lua_setglobal(L, STR_COLOR_GREEN);

    lua_pushinteger(L, TFT_BLUE);
    lua_setglobal(L, STR_COLOR_BLUE);

    lua_pushinteger(L, TFT_YELLOW);
    lua_setglobal(L, STR_COLOR_YELLOW);

    lua_pushinteger(L, TFT_ORANGE);
    lua_setglobal(L, STR_COLOR_ORANGE);
}

void deinit_color_constants(lua_State *L)
{
    lua_pushnil(L);
    lua_setglobal(L, STR_COLOR_BLACK);

    lua_pushnil(L);
    lua_setglobal(L, STR_COLOR_WHITE);

    lua_pushnil(L);
    lua_setglobal(L, STR_COLOR_RED);

    lua_pushnil(L);
    lua_setglobal(L, STR_COLOR_GREEN);

    lua_pushnil(L);
    lua_setglobal(L, STR_COLOR_BLUE);

    lua_pushnil(L);
    lua_setglobal(L, STR_COLOR_YELLOW);

    lua_pushnil(L);
    lua_setglobal(L, STR_COLOR_ORANGE);
}

int lua_iwgt_forced_draw(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    widget->forcedDraw();
    return 0;
}

int lua_iwgt_set_pos(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    uint16_t x = luaL_checknumber(L, 2);
    uint16_t y = luaL_checknumber(L, 3);
    widget->setPos(x, y);
    return 0;
}

int lua_iwgt_set_height(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    uint16_t h = luaL_checknumber(L, 2);
    widget->setHeight(h);
    return 0;
}

int lua_iwgt_get_hight(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    uint16_t h = widget->getHeight();
    lua_pushinteger(L, h);
    return 1;
}

int lua_iwgt_set_width(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    uint16_t w = luaL_checknumber(L, 2);
    widget->setWidth(w);
    return 0;
}

int lua_iwgt_get_width(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    uint16_t w = widget->getWidth();
    lua_pushinteger(L, w);
    return 1;
}

int lua_iwgt_set_corner_radius(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    uint16_t r = luaL_checkinteger(L, 2);
    widget->setCornerRadius(r);
    return 0;
}

int lua_iwgt_set_border(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    bool state = lua_toboolean(L, 2);
    widget->setBorder(state);
    return 0;
}

int lua_iwgt_set_border_color(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    uint16_t c = luaL_checkinteger(L, 2);
    widget->setBorderColor(c);
    return 0;
}

int lua_iwgt_get_x_pos(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    uint16_t x = widget->getXPos();
    lua_pushinteger(L, x);
    return 1;
}

int lua_iwgt_get_y_pos(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    uint16_t y = widget->getYPos();
    lua_pushinteger(L, y);
    return 1;
}

int lua_iwgt_get_id(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    uint16_t id = widget->getID();
    lua_pushinteger(L, id);
    return 1;
}

int lua_iwgt_set_changing_border(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    bool state = lua_toboolean(L, 2);
    widget->setChangingBorder(state);
    return 0;
}

int lua_iwgt_set_changing_back(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    bool state = lua_toboolean(L, 2);
    widget->setChangingBack(state);
    return 0;
}

int lua_iwgt_set_focus_border_color(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    uint16_t c = luaL_checkinteger(L, 2);
    widget->setFocusBorderColor(c);
    return 0;
}

int lua_iwgt_set_focus_back_color(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    uint16_t c = luaL_checkinteger(L, 2);
    widget->setFocusBackColor(c);
    return 0;
}

int lua_iwgt_set_focus(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    widget->setFocus();
    return 0;
}

int lua_iwgt_remove_focus(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    widget->removeFocus();
    return 0;
}

int lua_iwgt_set_visibility(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    uint16_t raw_value = luaL_checkinteger(L, 2);
    if (raw_value > IWidget::INVISIBLE)
        return luaL_error(L, "Invalid visibility value: %d", raw_value);

    IWidget::Visibility visibility = static_cast<IWidget::Visibility>(raw_value);
    return 0;
}

int lua_iwgt_set_transparency(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    bool state = lua_toboolean(L, 2);
    widget->setTransparency(state);
    return 0;
}

int lua_iwgt_set_back_color(lua_State *L)
{
    IWidget *widget = *(IWidget **)lua_touserdata(L, 1);
    uint16_t color = luaL_checkinteger(L, 2);
    widget->setBackColor(color);
    return 0;
}

// -------------------------------------------------------------------------------------------------------------

const struct luaL_Reg TYPE_METH_IWIDGET[] = {
    {"forcedDraw", lua_iwgt_forced_draw},
    {"setPos", lua_iwgt_set_pos},
    {"setHeight", lua_iwgt_set_height},
    {"getHeight", lua_iwgt_get_hight},
    {"setWidth", lua_iwgt_set_width},
    {"getWidth", lua_iwgt_get_width},
    {"setBackColor", lua_iwgt_set_back_color},
    {"setCornerRadius", lua_iwgt_set_corner_radius},
    {"setBorder", lua_iwgt_set_border},
    {"setBorderColor", lua_iwgt_set_border_color},
    {"getXPos", lua_iwgt_get_x_pos},
    {"getYPos", lua_iwgt_get_y_pos},
    {"getID", lua_iwgt_get_id},
    {"setChangingBorder", lua_iwgt_set_changing_border},
    {"setChangingBack", lua_iwgt_set_changing_back},
    {"setFocusBorderColor", lua_iwgt_set_focus_border_color},
    {"setFocusBackColor", lua_iwgt_set_focus_back_color},
    {"setFocus", lua_iwgt_set_focus},
    {"removeFocus", lua_iwgt_remove_focus},
    {"setVisibility", lua_iwgt_set_visibility},
    {"setTransparency", lua_iwgt_set_transparency},
    {nullptr, nullptr},
};

void lua_init_iwidget(lua_State *L)
{
    luaL_newmetatable(L, STR_TYPE_NAME_IWIDGET);
    lua_newtable(L);
    luaL_setfuncs(L, TYPE_METH_IWIDGET, 0);
    lua_setfield(L, -2, STR_LUA_INDEX);
    lua_pop(L, 1);

    init_iwidget_constants(L);
    init_color_constants(L);
}
