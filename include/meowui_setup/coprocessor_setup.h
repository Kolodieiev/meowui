#pragma once
#include <stdint.h>

// #define HAS_COPROCESSOR // Розкоментуй, якщо на платі використовуюється МК для розширення функціоналу.

#ifdef HAS_COPROCESSOR

typedef enum : uint8_t // Перечислення містить список команд для керування іншими МК по шині I2C.
{
    CCPU_CMD_ENABLE = 0,
    CCPU_CMD_DISABLE,
    CCPU_CMD_PIN_ON,
    CCPU_CMD_PIN_OFF,
    CCPU_CMD_GET_BTNS_STATE,
    CCPU_CMD_BTN_OFF,
    CCPU_CMD_BTN_ON,
} CoprocessorCMD_t;

#define COPROCESSOR_ADDR 0x66 // I2C - адреса МК, який відповідає за розширення функціоналу основного.

#endif // HAS_COPROCESSOR