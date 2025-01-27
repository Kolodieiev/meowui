#pragma once
#include <stdint.h>

// -------------------------------- Додай ID контекстів
namespace meow
{
    enum ContextID : uint8_t
    {
        ID_CONTEXT_SPLASH = 0,
        ID_CONTEXT_HOME,
        ID_CONTEXT_MENU,
        ID_CONTEXT_TORCH,
        ID_CONTEXT_GAMES,
        ID_CONTEXT_MP3,
        ID_CONTEXT_FILES,
        ID_CONTEXT_FIRMWARE,
        ID_CONTEXT_CALC,
        ID_CONTEXT_READER,
        // Контексти налаштувань
        ID_CONTEXT_PREF_SEL,
        ID_CONTEXT_PREF_BRIGHT,
        ID_CONTEXT_PREF_WATCH,
        ID_CONTEXT_PREF_WALL,
        ID_CONTEXT_PREF_FILE_SERVER,
        // Ігрові контексти
        ID_CONTEXT_SNAKE,
        ID_CONTEXT_SOKOBAN,
        ID_CONTEXT_TEST_SERVER,
        ID_CONTEXT_SIMPLE_RPG,
        //
    };
}
