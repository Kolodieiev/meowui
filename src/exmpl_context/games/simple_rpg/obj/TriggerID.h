// Перечислення ідентифікаторів тригерів, які можуть бути викорастані в сценах

#pragma once
#include <Arduino.h>

namespace simple_rpg
{
    enum TriggerID : int16_t
    {
        TRIGG_PORTAL_USED = 0,
        TRIGG_GAME_FINISHED,
    };
}