#define DISABLE_ALL_LIBRARY_WARNINGS // Вимкнути попередження бібліотеки.
#define USER_SETUP_ID 304
#define ST7789_DRIVER         // Configure all registers
#define TFT_RGB_ORDER TFT_BGR // Colour order Blue-Green-Red
#define SPI_FREQUENCY 80000000
//---------------------------------------------------
#define TFT_WIDTH 240
#define TFT_HEIGHT 320

// #define TFT_WIDTH 128
// #define TFT_HEIGHT 128

// #define TFT_WIDTH 240
// #define TFT_HEIGHT 240

// ---------------------------------------------------
#define LOAD_FONT2
#define LOAD_FONT4

//---------------------------------------------------
// Generic ESP32 setup
// #define TFT_MOSI 23 // sda
// #define TFT_SCLK 18 // scl
// #define TFT_DC 4
// #define TFT_CS 27

// ESP32 S3
#define USE_HSPI_PORT
#define TFT_MOSI 42 // sda
#define TFT_SCLK 41 // scl
#define TFT_DC 40
//
#define TFT_RST 3 // Стара схема
// #define TFT_RST 18 

// Позбутися помилки. 46 пін не використовується
#define TFT_CS 46
#define TFT_MISO 46 