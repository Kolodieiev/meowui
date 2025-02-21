#pragma once

#include <Arduino.h>
#include "DS3231.h"

namespace meow
{
    struct DS3231AlarmTime
    {
        uint8_t hour;
        uint8_t minute;
    };

    class DS3231Alarm
    {

    public:
        bool isEnabled();
        bool isAlarmed();
        bool setAlarmData(const DS3231AlarmTime &alarmData);
        DS3231AlarmTime getAlarmTime();
        bool enable(bool enableWithoutExternalPower);
        bool disable();
        bool procAlarm();

    private:
        const uint8_t REG_ALARMTWO = 0x0B;
        const uint8_t REG_ALARMTWO_SIZE = 3;

        // control bits
        const uint8_t DS3231_CNTRL_BIT_A2IE = 1;

        // status bits
        const uint8_t DS3231_A1F = 0;
        const uint8_t DS3231_A2F = 1;
        const uint8_t DS3231_AIFMASK = (_BV(DS3231_A1F) | _BV(DS3231_A2F));
        //
        I2C_Manager _i2c;
    };
}