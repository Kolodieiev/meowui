#pragma once
#include <stdint.h>

#include "meow/driver/graphics/GraphicsDriver.h"
#include "meowui_setup/context_id.h"

// -------------------------------- Підключи нижче заголовкові файли контекстів першого рівня
#include "exmpl_context/splash/SplashContext.h"
#include "exmpl_context/home/HomeContext.h"
#include "exmpl_context/menu/MenuContext.h"
#include "exmpl_context/games/GamesListContext.h"
#include "exmpl_context/mp3/Mp3Context.h"
#include "exmpl_context/reader/ReaderContext.h"
#include "exmpl_context/files/FilesContext.h"
#include "exmpl_context/preferences/PrefSelectContext.h"
#include "exmpl_context/preferences/PrefBrightContext.h"
#include "exmpl_context/preferences/PrefWatchContext.h"
#include "exmpl_context/preferences/PrefFileServerContext.h"
#include "exmpl_context/firmware/FirmwareContext.h"
#include "exmpl_context/wifi/WiFiContext.h"
// Ігрові контексти
#include "exmpl_context/games/sokoban/SokobanContext.h"
#include "exmpl_context/games/test_server/TestServerContext.h"
#include "exmpl_context/games/simple_rpg/SimpleRpgContext.h"

// -------------------------------- Додай перемикання контексту за прикладом
#define SCREEN_CASES                                    \
    case ContextID::ID_CONTEXT_HOME:                    \
        context = new HomeContext();                    \
        break;                                          \
    case ContextID::ID_CONTEXT_MENU:                    \
        context = new MenuContext();                    \
        break;                                          \
    case ContextID::ID_CONTEXT_GAMES:                   \
        context = new GamesListContext();               \
        break;                                          \
    case ContextID::ID_CONTEXT_MP3:                     \
        context = new Mp3Context();                     \
        break;                                          \
    case ContextID::ID_CONTEXT_READER:                  \
        context = new ReaderContext();                  \
        break;                                          \
    case ContextID::ID_CONTEXT_FILES:                   \
        context = new FilesContext();                   \
        break;                                          \
    case ContextID::ID_CONTEXT_PREF_SEL:                \
        context = new PrefSelectContext();              \
        break;                                          \
    case ContextID::ID_CONTEXT_PREF_BRIGHT:             \
        context = new PrefBrightContext();              \
        break;                                          \
    case ContextID::ID_CONTEXT_PREF_FILE_SERVER:        \
        context = new PrefFileServerContext();          \
        break;                                          \
    case ContextID::ID_CONTEXT_PREF_WATCH:              \
        context = new PrefWatchContext();               \
        break;                                          \
    case ContextID::ID_CONTEXT_FIRMWARE:                \
        context = new FirmwareContext();                \
        break;                                          \
    case ContextID::ID_CONTEXT_SOKOBAN:                 \
        context = new sokoban::SokobanContext();        \
        break;                                          \
    case ContextID::ID_CONTEXT_TEST_SERVER:             \
        context = new test_server::TestServerContext(); \
        break;                                          \
    case ContextID::ID_CONTEXT_SIMPLE_RPG:              \
        context = new simple_rpg::SimpleRpgContext();   \
        break;                                          \
    case ContextID::ID_CONTEXT_WIFI:                    \
        context = new WiFiContext();                    \
        break;

// -------------------------------- Стартовий контекст
#define START_CONTEXT SplashContext
