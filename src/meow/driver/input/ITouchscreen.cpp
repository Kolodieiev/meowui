#pragma GCC optimize("O3")
#include "ITouchscreen.h"

namespace meow
{
    void ITouchscreen::reset()
    {
        _is_holded = false;
        _is_pressed = false;
        _is_released = false;
        _is_swiped = false;
        _swipe = SWIPE_NONE;
        _x_s = 0;
        _y_s = 0;
        _x_e = 0;
        _y_e = 0;
    }

    void ITouchscreen::lock(unsigned long lock_duration)
    {
        _is_locked = true;
        _is_holded = false;
        _is_released = false;
        _is_pressed = false;
        _has_touch = false;
        _is_swiped = false;
        _lock_duration = lock_duration;
        _lock_time = millis();
    }
}