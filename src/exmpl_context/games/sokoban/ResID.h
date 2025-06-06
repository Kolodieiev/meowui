#pragma once
#include <Arduino.h>

namespace sokoban
{
    const char FX_PATH_STEP_1[] = "/game/sokoban/audio/fx/s0.wav";
    const char FX_PATH_STEP_2[] = "/game/sokoban/audio/fx/s1.wav";
    const char FX_PATH_STEP_3[] = "/game/sokoban/audio/fx/s2.wav";
    const char FX_PATH_STEP_4[] = "/game/sokoban/audio/fx/s3.wav";
    const char FX_PATH_STEP_5[] = "/game/sokoban/audio/fx/s4.wav";

    enum FxID : uint8_t
    {
        FX_ID_STEP_1 = 0,
        FX_ID_STEP_2,
        FX_ID_STEP_3,
        FX_ID_STEP_4,
        FX_ID_STEP_5
    };

}
// enum BmpID : uint8_t
// {

// };