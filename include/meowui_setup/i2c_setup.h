#pragma once
#include <stdint.h>

#define PIN_I2C_SDA (uint8_t)12
#define PIN_I2C_SCL (uint8_t)13

#define I2C_AWAIT_TIME_MS (unsigned long)100 // Час очікування відповіді від i2c пристрою

typedef enum : uint8_t // Перечислення містить список команд для керування іншими МК по шині I2C.
{
    I2C_CMD_ENABLE = 0,
    I2C_CMD_DISABLE,
    I2C_CMD_PIN_ON,
    I2C_CMD_PIN_OFF,
    I2C_CMD_BTNS_STATE,
    I2C_CMD_OFF_BTN,
    I2C_CMD_ON_BTN,
} ExtI2CMD_t;