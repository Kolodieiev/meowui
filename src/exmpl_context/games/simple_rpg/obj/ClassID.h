#pragma once
#include <Arduino.h>

namespace simple_rpg
{
    // Перечислення, в якому зберігаються ідентифікатори типів ігрових об'єктів
    enum ClassID : uint8_t
    {
        CLASS_HERO = 0,
        CLASS_PORTAL,
        CLASS_BUNNY
    };
}