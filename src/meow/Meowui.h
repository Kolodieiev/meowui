#pragma once
#include <Arduino.h>

namespace meow
{
    class Meowui
    {
    public:
        /**
         * @brief Виконує початкову ініціалізацію фреймворку та запускає задачу менеджера контекстів.
         *
         * @param stack_depth_kb Розмір стеку Meowui в кілобайтах.
         * Пам'ять цього стеку буде використовуватися на потреби фреймворку, а також під час виконання будь-якого із контекстів.
         */
        void begin(uint32_t stack_depth_kb);

    private:
        static void meowContextTask(void *params);
    };

    extern Meowui MEOW;
}
