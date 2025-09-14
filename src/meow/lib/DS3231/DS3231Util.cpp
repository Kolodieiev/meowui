#include <Arduino.h>
#include "DS3231Util.h"

namespace meow
{

    uint8_t bcdToUint8(uint8_t val)
    {
        return val - 6 * (val >> 4);
    }

    uint8_t uint8ToBcd(uint8_t val)
    {
        return val + 6 * (val / 10);
    }

    uint8_t bcdToBin24Hour(uint8_t bcdHour)
    {
        uint8_t hour;
        if (bcdHour & 0x40)
        {
            bool isPm = ((bcdHour & 0x20) != 0);

            hour = bcdToUint8(bcdHour & 0x1f);
            if (isPm)
                hour += 12;
        }
        else
            hour = bcdToUint8(bcdHour);

        return hour;
    }

    uint8_t strToUint8(const char *pString)
    {
        uint8_t value = 0;

        while ('0' == *pString || *pString == ' ')
        {
            pString++;
        }

        while ('0' <= *pString && *pString <= '9')
        {
            value *= 10;
            value += *pString - '0';
            pString++;
        }

        return value;
    }
}