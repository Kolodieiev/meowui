// Перечислення ідентифікаторів тригерів, які можуть бути викорастані в сценах

#pragma once
#include <Arduino.h>

namespace test_server
{
    enum TriggerID : int16_t
    {
        TRIGGER_NEXT_SCENE = 0,
        TRIGGER_GAME_FINISHED,
        TRIGGER_GAME_LOST,
    };
}