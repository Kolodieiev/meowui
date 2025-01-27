#pragma once
#include <stdint.h>

#include "meow/driver/graphics/GraphicsDriver.h"
#include "meowui_setup/context_id.h"

// -------------------------------- Підключи нижче заголовкові файли контекстів першого рівня
#include "context/splash/SplashContext.h"
#include "context/home/HomeContext.h"
#include "context/menu/MenuContext.h"
#include "context/games/GamesListContext.h"
#include "context/mp3/Mp3Context.h"
#include "context/reader/ReaderContext.h"
#include "context/files/FilesContext.h"
#include "context/preferences/PrefSelectContext.h"
#include "context/preferences/PrefBrightContext.h"
#include "context/preferences/PrefWatchContext.h"
#include "context/preferences/PrefFileServerContext.h"
#include "context/firmware/FirmwareContext.h"
// #include "context/calc/CalcContext.h"
// Ігрові контексти
#include "context/games/sokoban/SokobanContext.h"
#include "context/games/test_server/TestServerContext.h"
#include "context/games/simple_rpg/SimpleRpgContext.h"

// -------------------------------- Додай перемикання контексту за прикладом
#define SCREEN_CASES                                            \
    case ContextID::ID_CONTEXT_HOME:                            \
        context = new HomeContext(_display);                    \
        break;                                                  \
    case ContextID::ID_CONTEXT_MENU:                            \
        context = new MenuContext(_display);                    \
        break;                                                  \
    case ContextID::ID_CONTEXT_GAMES:                           \
        context = new GamesListContext(_display);               \
        break;                                                  \
    case ContextID::ID_CONTEXT_MP3:                             \
        context = new Mp3Context(_display);                     \
        break;                                                  \
    case ContextID::ID_CONTEXT_READER:                          \
        context = new ReaderContext(_display);                  \
        break;                                                  \
    case ContextID::ID_CONTEXT_FILES:                           \
        context = new FilesContext(_display);                   \
        break;                                                  \
    case ContextID::ID_CONTEXT_PREF_SEL:                        \
        context = new PrefSelectContext(_display);              \
        break;                                                  \
    case ContextID::ID_CONTEXT_PREF_BRIGHT:                     \
        context = new PrefBrightContext(_display);              \
        break;                                                  \
    case ContextID::ID_CONTEXT_PREF_FILE_SERVER:                \
        context = new PrefFileServerContext(_display);          \
        break;                                                  \
    case ContextID::ID_CONTEXT_PREF_WATCH:                      \
        context = new PrefWatchContext(_display);               \
        break;                                                  \
    case ContextID::ID_CONTEXT_FIRMWARE:                        \
        context = new FirmwareContext(_display);                \
        break;                                                  \
    case ContextID::ID_CONTEXT_SOKOBAN:                         \
        context = new sokoban::SokobanContext(_display);        \
        break;                                                  \
    case ContextID::ID_CONTEXT_TEST_SERVER:                     \
        context = new test_server::TestServerContext(_display); \
        break;                                                  \
    case ContextID::ID_CONTEXT_SIMPLE_RPG:                      \
        context = new simp_rpg::SimpleRpgContext(_display);     \
        break;

// -------------------------------- Стартовий контекст
#define START_CONTEXT SplashContext
// #define START_CONTEXT test_server::TestServerContext
