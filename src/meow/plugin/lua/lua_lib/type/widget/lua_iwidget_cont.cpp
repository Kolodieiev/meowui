#include "lua_iwidget_cont.h"
#include "./lua_iwidget.h"
#include "meow/ui/widget/IWidgetContainer.h"
#include "meow/plugin/lua/res/lua_strs.h"
#include "meow/plugin/lua/res/lua_err_msg.h"
#include "vector"

using namespace meow;

std::vector<const char *> _iwidget_cont_type_name_list;

void lua_push_widget_or_nil(lua_State *L, IWidget *widget)
{
    if (!widget)
    {
        lua_pushnil(L);
    }
    else
    {
        IWidget **r_widget = (IWidget **)lua_newuserdata(L, sizeof(IWidget *));
        *r_widget = widget;
        luaL_getmetatable(L, STR_TYPE_NAME_IWIDGET);
        lua_setmetatable(L, -2);
    }
}

int lua_cont_add_widget(lua_State *L)
{
    IWidgetContainer *container = *(IWidgetContainer **)lua_touserdata(L, 1);
    IWidget *widget = *(IWidget **)lua_touserdata(L, 2);
    container->addWidget(widget);
    return 0;
}

int lua_cont_delete_widget_by_id(lua_State *L)
{
    IWidgetContainer *container = *(IWidgetContainer **)lua_touserdata(L, 1);
    uint16_t id = luaL_checkinteger(L, 2);
    bool result = container->deleteWidgetByID(id);
    lua_pushboolean(L, result);
    return 1;
}

int lua_cont_find_widget_by_id(lua_State *L)
{
    IWidgetContainer *container = *(IWidgetContainer **)lua_touserdata(L, 1);
    uint16_t id = luaL_checkinteger(L, 2);
    IWidget *widget = container->findWidgetByID(id);
    lua_push_widget_or_nil(L, widget);
    return 1;
}

int lua_cont_find_widget_by_indx(lua_State *L)
{
    IWidgetContainer *container = *(IWidgetContainer **)lua_touserdata(L, 1);
    uint16_t indx = luaL_checkinteger(L, 2);
    IWidget *widget = container->findWidgetByID(indx);
    lua_push_widget_or_nil(L, widget);
    return 1;
}

int lua_cont_get_widget_by_coords(lua_State *L)
{
    IWidgetContainer *container = *(IWidgetContainer **)lua_touserdata(L, 1);
    uint16_t x = luaL_checkinteger(L, 2);
    uint16_t y = luaL_checkinteger(L, 3);
    IWidget *widget = container->getWidgetByCoords(x, y);
    lua_push_widget_or_nil(L, widget);
    return 1;
}

int lua_cont_delete_widgets(lua_State *L)
{
    IWidgetContainer *container = *(IWidgetContainer **)lua_touserdata(L, 1);
    container->deleteWidgets();
    return 0;
}

int lua_cont_get_size(lua_State *L)
{
    IWidgetContainer *container = *(IWidgetContainer **)lua_touserdata(L, 1);
    uint16_t size = container->getSize();
    lua_pushinteger(L, size);
    return 1;
}

int lua_cont_enable(lua_State *L)
{
    IWidgetContainer *container = *(IWidgetContainer **)lua_touserdata(L, 1);
    container->enable();
    return 0;
}

int lua_cont_disable(lua_State *L)
{
    IWidgetContainer *container = *(IWidgetContainer **)lua_touserdata(L, 1);
    container->disable();
    return 0;
}

// -------------------------------------------------------------------------------------------------------------

const struct luaL_Reg TYPE_METH_IWIDGET_CONT[] = {
    {"addWidget", lua_cont_add_widget},
    {"deleteWidgetByID", lua_cont_delete_widget_by_id},
    {"findWidgetByID", lua_cont_find_widget_by_id},
    {"getWidgetByIndx", lua_cont_find_widget_by_indx},
    {"getWidgetByCoords", lua_cont_get_widget_by_coords},
    {"deleteWidgets", lua_cont_delete_widgets},
    {"getSize", lua_cont_get_size},
    {"enable", lua_cont_enable},
    {"disable", lua_cont_disable},
    {nullptr, nullptr},
};

void lua_init_iwidget_cont(lua_State *L, const char *type_caller_name)
{
    if (!type_caller_name)
    {
        log_e("%s", STR_EMPTY_MODULE_NAME);
        esp_restart();
    }

    if (_iwidget_cont_type_name_list.empty())
    {
        lua_init_iwidget(L, STR_TYPE_NAME_IWIDGET_CONT);

        luaL_newmetatable(L, STR_TYPE_NAME_IWIDGET_CONT);
        luaL_setfuncs(L, TYPE_METH_IWIDGET_CONT, 0);
        luaL_getmetatable(L, STR_TYPE_NAME_IWIDGET);
        lua_setfield(L, -2, STR_LUA_INDEX);
        lua_pop(L, 1);
    }

    _iwidget_cont_type_name_list.push_back(type_caller_name);
}

void lua_deinit_iwidget_cont(lua_State *L, const char *type_caller_name)
{
    if (!type_caller_name)
    {
        log_e("%s", STR_EMPTY_MODULE_NAME);
        esp_restart();
    }

    for (auto it_b = _iwidget_cont_type_name_list.begin(), it_e = _iwidget_cont_type_name_list.end(); it_b != it_e; ++it_b)
    {
        if (strcmp(*it_b, type_caller_name) == 0)
        {
            _iwidget_cont_type_name_list.erase(it_b);
            break;
        }
    }

    if (_iwidget_cont_type_name_list.empty())
    {
        lua_pushnil(L);
        lua_setfield(L, LUA_REGISTRYINDEX, STR_TYPE_NAME_IWIDGET_CONT);
        lua_deinit_iwidget(L, STR_TYPE_NAME_IWIDGET_CONT);
    }
}

void lua_clear_iwidget_cont()
{
    _iwidget_cont_type_name_list.clear();
}
