#pragma once
#include <stdint.h>

//  Знайти налаштування для дисплеїв можна у папкі lib/TFT_eSPI/User_Setups

#define DISABLE_ALL_LIBRARY_WARNINGS // Вимкнути попередження бібліотеки.
#define USER_SETUP_ID 304
#define ST7789_DRIVER
#define TFT_RGB_ORDER TFT_BGR
// #define SPI_FREQUENCY 40000000
#define SPI_FREQUENCY 80000000

#define TFT_WIDTH 240
#define TFT_HEIGHT 280
#define TFT_CUTOUT 36

#define LOAD_FONT2
#define LOAD_FONT4

// Лілка
#define USE_HSPI_PORT
//
#define TFT_DC 15
#define TFT_CS 7
#define TFT_MOSI 17 // sda
#define TFT_SCLK 18 // dcl
#define TFT_RST -1  // TFT reset pin connect to RST

// Позбутися помилки. 8 пін використовується для SD_MISO
#define TFT_MISO 8

// #define BACKLIGHT_PIN (uint8_t)46  // Пін управління підсвіткою дисплея. Закоментуй, якщо відсутнє управління підсвіткою
#define PWM_FREQ (uint16_t)3000 // Частота PWM підсвітки дисплея
#define PWM_CHANEL (uint8_t)0
#define PWM_RESOLUTION (uint8_t)8
