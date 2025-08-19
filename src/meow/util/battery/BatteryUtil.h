#pragma once
#include <Arduino.h>
#include <meowui_setup/battery_setup.h>

namespace meow
{
#ifdef HAS_BATTERY

#if !defined(PIN_VOLT_MEASH) || !defined(R_DIV_K) || !defined(VOLTAGE_SAMP_NUM)
#error "Одине з необхідних налаштувань акумулятора не вказано!"
#endif

    static_assert(R_DIV_K != 0, "R_DIV_K must not be 0");
    static_assert(VOLTAGE_SAMP_NUM != 0, "VOLTAGE_SAMP_NUM must not be 0");
    static_assert(PIN_VOLT_MEASH < 60 && PIN_VOLT_MEASH > 0, "PIN_VOLT_MEASH must be 60 > PIN_VOLT_MEASH > 0");

    class BatteryUtil
    {
    public:
        /**
         * @brief Зчитує напругу на акумуляторі, відповідно до вказаних налаштувань.
         *
         * @return float
         */
        static float readVoltVal();

    private:
    };

#endif // HAS_BATTERY
}