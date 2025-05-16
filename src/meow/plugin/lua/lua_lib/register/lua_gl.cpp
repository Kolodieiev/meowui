#include "lua_gl.h"
#include <Arduino.h>
#include "meow/driver/graphics/GraphicsDriver.h"
#include "meow/plugin/lua/lua_lib/helper/lua_helper.h"

using namespace meow;

const char STR_LIB_NAME_GL[] = "gl";

int lua_gl_width(lua_State *L)
{
    lua_pushinteger(L, _display.width());
    return 1;
}

int lua_gl_height(lua_State *L)
{
    lua_pushinteger(L, _display.height());
    return 1;
}

int lua_gl_cutout(lua_State *L)
{
#if defined(TFT_CUTOUT)
    lua_pushinteger(L, TFT_CUTOUT);
#else
    lua_pushinteger(L, 0);
#endif
    return 1;
}

int lua_gl_draw_rect(lua_State *L)
{
    if (lua_check_top(L, {5}) == 0)
        return 0;

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    int c = luaL_checkinteger(L, 5);

    _display.drawRect(x, y, w, h, c);
    return 0;
}

int lua_gl_fill_rect(lua_State *L)
{
    if (lua_check_top(L, {5}) == 0)
        return 0;

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    int c = luaL_checkinteger(L, 5);

    _display.fillRect(x, y, w, h, c);
    return 0;
}

int lua_gl_draw_r_rect(lua_State *L)
{
    if (lua_check_top(L, {6}) == 0)
        return 0;

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    int r = luaL_checkinteger(L, 5);
    int c = luaL_checkinteger(L, 6);

    _display.drawRoundRect(x, y, w, h, r, c);
    return 1;
}

int lua_gl_fill_r_rect(lua_State *L)
{
    if (lua_check_top(L, {6}) == 0)
        return 0;

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    int r = luaL_checkinteger(L, 5);
    int c = luaL_checkinteger(L, 6);

    _display.fillRoundRect(x, y, w, h, r, c);
    return 1;
}

int lua_gl_set_text_font(lua_State *L)
{
    int font_id = luaL_checkinteger(L, 1);
    _display.setTextFont(font_id);
    return 0;
}

int lua_gl_set_text_size(lua_State *L)
{
    int txt_size = luaL_checkinteger(L, 1);
    _display.setTextSize(txt_size);
    return 0;
}

int lua_gl_set_text_color(lua_State *L)
{
    int color = luaL_checkinteger(L, 1);
    _display.setTextColor(color);
    return 0;
}

int lua_gl_draw_string(lua_State *L)
{
    if (lua_check_top(L, {3}) == 0)
        return 0;

    const char *str = luaL_checkstring(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    _display.drawString(str, x, y);
    return 0;
}

//----------------------------------------------------------------------------------------------------

const struct luaL_Reg LIB_GL[] = {
    {"width", lua_gl_width},
    {"height", lua_gl_height},
    {"cutout", lua_gl_cutout},
    {"drawRect", lua_gl_draw_rect},
    {"fillRect", lua_gl_fill_rect},
    {"drawRoundRect", lua_gl_draw_r_rect},
    {"fillRoundRect", lua_gl_fill_r_rect},
    {"setTextFont", lua_gl_set_text_font},
    {"setTextSize", lua_gl_set_text_size},
    {"setTextColor", lua_gl_set_text_color},
    {"drawString", lua_gl_draw_string},
    {nullptr, nullptr},
};

int lua_register_gl(lua_State *L)
{
    luaL_newlib(L, LIB_GL);
    lua_setglobal(L, STR_LIB_NAME_GL);
    return 0;
}