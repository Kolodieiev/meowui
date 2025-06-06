// ST7789 240 x 280 display with no chip select line
#define USER_SETUP_ID 203

#define ST7789_DRIVER     // Configure all registers

#define TFT_WIDTH  240
#define TFT_HEIGHT 280

#define CGRAM_OFFSET      // Library will add offsets required

//#define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue
//#define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

//#define TFT_INVERSION_ON
//#define TFT_INVERSION_OFF

// DSTIKE stepup
//#define TFT_DC    23
//#define TFT_RST   32
//#define TFT_MOSI  26
//#define TFT_SCLK  27

// Generic ESP32 setup
//#define TFT_MISO 19
//#define TFT_MOSI 23
//#define TFT_SCLK 18
//#define TFT_CS    -1 // Not connected
//#define TFT_DC    2
//#define TFT_RST   4  // Connect reset to ensure display initialises

// For NodeMCU - use pin numbers in the form PIN_Dx where Dx is the NodeMCU pin designation
#define TFT_CS   -1      // Define as not used
#define TFT_DC   PIN_D1  // Data Command control pin
//#define TFT_RST  PIN_D4  // TFT reset pin (could connect to NodeMCU RST, see next line)
#define TFT_RST  -1      // TFT reset pin connect to NodeMCU RST, must also then add 10K pull down to TFT SCK



#define LOAD_FONT2  
#define LOAD_FONT4 


// #define SPI_FREQUENCY  27000000
#define SPI_FREQUENCY  40000000

#define SPI_READ_FREQUENCY  20000000

#define SPI_TOUCH_FREQUENCY  2500000

// #define SUPPORT_TRANSACTIONS