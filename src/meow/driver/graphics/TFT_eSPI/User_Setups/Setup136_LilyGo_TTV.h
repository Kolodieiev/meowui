// ST7789 135 x 240 display with no chip select line
#define USER_SETUP_ID 136

#define ST7789_DRIVER     // Configure all registers

#define TFT_WIDTH  135
#define TFT_HEIGHT 240

#define CGRAM_OFFSET      // Library will add offsets required

// Generic ESP32 setup
//#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS    5 
#define TFT_DC    19
#define TFT_RST   4  



#define LOAD_FONT2  
#define LOAD_FONT4 


// #define SPI_FREQUENCY  27000000
#define SPI_FREQUENCY  40000000

// #define SUPPORT_TRANSACTIONS