#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>

namespace meow
{
    class ITouchscreen
    {
    public:
        enum Swipe : uint8_t
        {
            SWIPE_NONE = 0,
            SWIPE_L,
            SWIPE_R,
            SWIPE_D,
            SWIPE_U,
        };

        virtual ~ITouchscreen() {}

        virtual void setRotation(uint8_t rotation) = 0;
        virtual void __update() = 0;

        void reset();

        bool isHolded() const { return _is_holded; }
        bool isPressed() const { return _is_pressed; }
        bool isReleased() const { return _is_released; }
        bool isSwiped() const { return _is_swiped; }
        Swipe getSwipe() const { return _swipe; }

        void lock(unsigned long lock_duration);

        uint16_t getX() const { return _x_s; }
        uint16_t getY() const { return _y_s; }

    protected:
        uint16_t _width;
        uint16_t _height;
        uint16_t _x_s = 0;
        uint16_t _y_s = 0;
        uint16_t _x_e = 0;
        uint16_t _y_e = 0;

        bool _is_locked = false;
        unsigned long _lock_time;
        unsigned long _lock_duration;

        unsigned long _hold_duration;

        bool _has_touch = false;
        bool _is_holded = false;
        bool _is_pressed = false;
        bool _is_released = false;
        bool _is_swiped = false;
        Swipe _swipe = SWIPE_NONE;
    };
}