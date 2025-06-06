// GC9A01 128 x 128 display with no chip select line
#define USER_SETUP_ID 211

#define GC9A01_DRIVER     // Configure all registers

#define TFT_WIDTH  128
#define TFT_HEIGHT 128

// #define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue
//#define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

// #define TFT_INVERSION_ON
//#define TFT_INVERSION_OFF
#define TFT_BACKLIGHT_ON 0
#define CGRAM_OFFSET


#define TFT_BL     10   // LED back-light
#define TFT_MISO   -1   // Not connected
#define TFT_MOSI   2
#define TFT_SCLK   3
#define TFT_CS     5 
#define TFT_DC     6
#define TFT_RST    1 // Connect reset to ensure display initialises


#define LOAD_FONT2  
#define LOAD_FONT4 


// #define SPI_FREQUENCY  27000000
#define SPI_FREQUENCY  40000000

#define SPI_READ_FREQUENCY  20000000

#define SPI_TOUCH_FREQUENCY  2500000

// #define SUPPORT_TRANSACTIONS