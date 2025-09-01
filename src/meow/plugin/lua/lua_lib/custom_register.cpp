#pragma GCC optimize("O3")
#include "custom_register.h"
//
#include "./register/lua_mcu.h"
#include "./register/lua_pin.h"
#include "./register/lua_gl.h"

// Додай сюди функції для глобальної реєстрації бібліотек
const LuaRegisterFunc LIB_REGISTER_FUNCS[] = {
    lua_register_mcu,
    lua_register_pin,
    lua_register_gl,
};

void register_custom_modules(lua_State *L)
{
    const size_t funcs_num = sizeof(LIB_REGISTER_FUNCS) / sizeof(LuaRegisterFunc);
    for (size_t i = 0; i < funcs_num; ++i)
        LIB_REGISTER_FUNCS[i](L);
}