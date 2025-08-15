#pragma once
#include <stdint.h>
#include <unordered_map>
#include <functional>
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

namespace meow
{
    // -------------------------------- Додай перемикання контексту за прикладом
    std::unordered_map<ContextID, std::function<IContext *()>> _context_id_map = {
        {ContextID::ID_CONTEXT_SPLASH, []()
         { return new SplashContext(); }},
        {ContextID::ID_CONTEXT_HOME, []()
         { return new HomeContext(); }},
        {ContextID::ID_CONTEXT_MENU, []()
         { return new MenuContext(); }},
        {ContextID::ID_CONTEXT_MP3, []()
         { return new Mp3Context(); }},
        {ContextID::ID_CONTEXT_FILES, []()
         { return new FilesContext(); }},
        {ContextID::ID_CONTEXT_GAMES, []()
         { return new GamesListContext(); }},
        {ContextID::ID_CONTEXT_PREF_SEL, []()
         { return new PrefSelectContext(); }},
        {ContextID::ID_CONTEXT_PREF_BRIGHT, []()
         { return new PrefBrightContext(); }},
        {ContextID::ID_CONTEXT_PREF_FILE_SERVER, []()
         { return new PrefFileServerContext(); }},
        {ContextID::ID_CONTEXT_PREF_WATCH, []()
         { return new PrefWatchContext(); }},
        {ContextID::ID_CONTEXT_READER, []()
         { return new ReaderContext(); }},
        {ContextID::ID_CONTEXT_FIRMWARE, []()
         { return new FirmwareContext(); }},
        {ContextID::ID_CONTEXT_SOKOBAN, []()
         { return new sokoban::SokobanContext(); }},
        {ContextID::ID_CONTEXT_TEST_SERVER, []()
         { return new test_server::TestServerContext(); }},
        {ContextID::ID_CONTEXT_SIMPLE_RPG, []()
         { return new simple_rpg::SimpleRpgContext(); }},
        {ContextID::ID_CONTEXT_WIFI, []()
         { return new WiFiContext(); }},
    };
}

// -------------------------------- Стартовий контекст
#define START_CONTEXT SplashContext
