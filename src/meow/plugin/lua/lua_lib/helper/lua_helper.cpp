#pragma GCC optimize("O3")
#include "lua_helper.h"

const char STR_INCORRECT_ARGS_NUMBER_BTW[] = "Некоректна кількість аргументів. Очікується [%s], отримано %d";

uint8_t lua_check_top(lua_State *L, std::initializer_list<uint8_t> args_assert)
{
    uint8_t args_num = lua_gettop(L);
    bool result = false;

    for (uint8_t a : args_assert)
    {
        if (args_num == a)
        {
            result = true;
            break;
        }
    }

    if (!result)
    {
        char expected_buf[64] = {0};
        char *ptr = expected_buf;
        for (uint8_t a : args_assert)
            ptr += snprintf(ptr, expected_buf + sizeof(expected_buf) - ptr, "%u,", a);
        if (ptr != expected_buf && ptr[-1] == ',')
            ptr[-1] = '\0';

        return luaL_error(L, STR_INCORRECT_ARGS_NUMBER_BTW, expected_buf, args_num);
    }
    else
        return args_num;
}
