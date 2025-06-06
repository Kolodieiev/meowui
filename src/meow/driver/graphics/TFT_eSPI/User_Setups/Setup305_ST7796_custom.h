#pragma once
#define DISABLE_ALL_LIBRARY_WARNINGS // Вимкнути попередження бібліотеки.
#define USER_SETUP_ID 305
#define ST7796_DRIVER
#define TFT_WIDTH 320
#define TFT_HEIGHT 480

// #define TFT_BL 27 // LED back-light control pin
// #define TFT_BACKLIGHT_ON HIGH // Level to turn ON back-light (HIGH or LOW)

#define TFT_MISO 12
#define TFT_MOSI 13 // In some display driver board, it might be written as "SDA" and so on.
#define TFT_SCLK 14
#define TFT_CS 15 // Chip select control pin
#define TFT_DC 2 // Data Command control pin
#define TFT_RST -1 // Reset pin (could connect to Arduino RESET pin)

// #define TOUCH_CS 33 // Chip select pin (T_CS) of touch screen

#define LOAD_FONT2 
#define LOAD_FONT4 

#define SPI_FREQUENCY 65000000
#define SPI_READ_FREQUENCY 20000000
#define SPI_TOUCH_FREQUENCY 2500000 //2500000