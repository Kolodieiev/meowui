#pragma once
#include <stdint.h>

#include "meow/driver/graphics/GraphicsDriver.h"
#include "meowui_setup/context_id.h"

// -------------------------------- Підключи нижче заголовкові файли контекстів першого рівня

//
// УВАГА, НИЖЧЕ КОД ДОДАНО ДЛЯ ПРИКЛАДУ!!! Ви повинні створити власні заголовкові файли контексту(екранів/модулів) для свого проєкту!
// Код у цьому репозиторії НЕ Є готовою універсальною прошивкою для всього!
//

#include "context/splash/SplashContext.h"
#include "context/menu/MenuContext.h"

// -------------------------------- Додай перемикання контексту за прикладом
#define SCREEN_CASES                 \
    case ContextID::ID_CONTEXT_MENU: \
        context = new MenuContext(); \
        break;
// case ContextID::ID_CONTEXT_HOME:
//     context = new HomeContext();
//     break;

// -------------------------------- Стартовий контекст
#define START_CONTEXT SplashContext
