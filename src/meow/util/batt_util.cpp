#include "batt_util.h"

namespace meow
{
#ifdef HAS_BATTERY

    float readBattVoltage()
    {
        float bat_voltage = 0.0f;

        for (uint8_t i{0}; i < VOLTAGE_SAMP_NUM; ++i)
            bat_voltage += analogRead(PIN_VOLT_MEASH);

        bat_voltage /= VOLTAGE_SAMP_NUM;
        bat_voltage *= 3.3;
        bat_voltage /= 4095;
        bat_voltage /= R_DIV_K;

        return bat_voltage;
    }

#endif // HAS_BATTERY
}