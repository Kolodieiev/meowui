#pragma once
#include <stdint.h>

#if defined(CONFIG_IDF_TARGET_ESP32S3)

// Налаштування пінів для Лілка_V2
#define PIN_LR_AUX 12
#define PIN_LR_TX 13
#define PIN_LR_RX 21
#define PIN_LR_M1 47
#define PIN_LR_M0 48

#else

#define PIN_LR_AUX 17
#define PIN_LR_TX 4
#define PIN_LR_RX 15
#define PIN_LR_M1 16
#define PIN_LR_M0 5

#endif
