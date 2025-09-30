#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>

namespace meow
{
    class Button
    {
    public:
        /**
         * @brief Створює новий об'єкт віртуального піна.
         *
         * @param btn_id Номер фізичного піна, до якого цей об'єкт буде прив'язано
         * @param is_touch Вказує спосіб ініціалізації піна.
         * Якщо true - пін буде ініціалізовано як сенсорний датчик.
         * Якщо false - пін буде ініціалізовано як тактову кнопку.
         */
        Button(uint8_t btn_id, bool is_touch);

        void lock(unsigned long lock_duration);
        void __update();
        void __extUpdate(bool is_holded);
        void reset();

        uint8_t getID() const { return _btn_id; }
        bool isHolded() const { return _is_holded; }
        bool isPressed() const { return _is_pressed; }
        bool isReleased() const { return _is_released; }

        bool operator<(const Button &other) const
        {
            return _btn_id < other.getID();
        }

        void enable();
        void disable();

    private:
        unsigned long _lock_time{0};
        unsigned long _lock_duration{0};
        unsigned long _hold_duration{0};
        //
        const uint8_t _btn_id;
        //
        const bool _is_touch;
        bool _is_locked{false};
        bool _is_touched{false};
        bool _is_holded{false};
        bool _is_pressed{false};
        bool _is_released{false};
        bool _is_enabled;

        void updateState();
    };
}