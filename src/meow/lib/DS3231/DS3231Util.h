#pragma once
#include <Arduino.h>
#include <inttypes.h>

namespace meow
{
#if !defined(ISR_ATTR)
#define ISR_ATTR ICACHE_RAM_ATTR
#endif

    extern uint8_t bcdToUint8(uint8_t val);
    extern uint8_t uint8ToBcd(uint8_t val);
    extern uint8_t bcdToBin24Hour(uint8_t bcdHour);
    extern uint8_t strToUint8(const char *pString);
}