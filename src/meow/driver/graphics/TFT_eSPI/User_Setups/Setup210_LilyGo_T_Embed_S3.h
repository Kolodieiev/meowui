// ST7789 170 x 320 display with no chip select line
#define USER_SETUP_ID 210

#define ST7789_DRIVER     // Configure all registers

#define TFT_WIDTH  170
#define TFT_HEIGHT 320

//#define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue
//#define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

#define TFT_INVERSION_ON
//#define TFT_INVERSION_OFF
#define TFT_BACKLIGHT_ON 1

#define TFT_BL     15   // LED back-light
#define TFT_MISO   -1   // Not connected
#define TFT_MOSI   11
#define TFT_SCLK   12
#define TFT_CS     10 
#define TFT_DC     13
#define TFT_RST    9 // Connect reset to ensure display initialises


#define LOAD_FONT2  
#define LOAD_FONT4 


// #define SPI_FREQUENCY  27000000
#define SPI_FREQUENCY  40000000

#define SPI_READ_FREQUENCY  20000000

#define SPI_TOUCH_FREQUENCY  2500000

// #define SUPPORT_TRANSACTIONS