#pragma once
#include <stdint.h>

// -------------------------------- Додай ID контекстів
namespace meow
{
    enum ContextID : uint8_t
    {
        ID_CONTEXT_SPLASH = 0,
        ID_CONTEXT_MENU,
        ID_CONTEXT_GAMES,
        ID_CONTEXT_MP3,
        ID_CONTEXT_FILES,
        ID_CONTEXT_FIRMWARE,
        ID_CONTEXT_READER,
        ID_CONTEXT_PREF_SEL,
    };
}
