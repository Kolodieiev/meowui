#pragma once
#include <stdint.h>

#define PIN_I2C_SDA (uint8_t)12
#define PIN_I2C_SCL (uint8_t)13

#define I2C_AWAIT_TIME_MS (unsigned long)100 // Час очікування відповіді від i2c пристрою