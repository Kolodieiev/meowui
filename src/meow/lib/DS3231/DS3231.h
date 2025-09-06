#pragma once
#include <Arduino.h>

#include "./DS3231DateTime.h"
#include "./DS3231ComnConst.h"
#include "meow/manager/I2C_Manager.h"

namespace meow
{
    class DS3231
    {
    public:
        bool begin();
        bool isDateTimeValid();
        bool isRunning();
        bool enable();
        bool disable();
        bool setDateTime(const DS3231DateTime &date_time);
        bool connected() const;
        DS3231DateTime getDateTime();
        uint8_t dayOfWeek(uint16_t year, uint8_t month, uint8_t day_of_month);
        DS3231DateTime compiledToDS3231DateTime(const char *date, const char *time);

    private:
        const uint8_t REG_TIMEDATE{0x00};
        const uint8_t REG_TIMEDATE_SIZE{7};
        const uint8_t CNTRL_BIT_EOSC{7};

        const uint8_t STS_BIT_BSY{2};
        const uint8_t STS_BIT_EN32KHZ{3};
        const uint8_t STS_BIT_OSF{7};

        I2C_Manager _i2c;
    };
}