#define USER_SETUP_ID 44

#define ST7789_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 240

#define TFT_MOSI    19
#define TFT_MISO    22
#define TFT_SCLK    21
#define TFT_CS      12
#define TFT_DC      15
#define TFT_RST     -1
#define TFT_BL 2


#define LOAD_FONT2  
#define LOAD_FONT4 

// #define SPI_FREQUENCY   1000000
// #define SPI_FREQUENCY   5000000
// #define SPI_FREQUENCY  10000000
// #define SPI_FREQUENCY  20000000
// #define SPI_FREQUENCY 27000000 // Actually sets it to 26.67MHz = 80/3
#define SPI_FREQUENCY  40000000 // Maximum to use SPIFFS
// #define SUPPORT_TRANSACTIONS
