// Config for two ST7735 128 x 128 displays for Animated_Eyes example
#define USER_SETUP_ID 47

#define ST7735_DRIVER     // Configure all registers

#define TFT_WIDTH  128
#define TFT_HEIGHT 128

// #define ST7735_INITB
// #define ST7735_GREENTAB
// #define ST7735_GREENTAB2
 #define ST7735_GREENTAB3
// #define ST7735_GREENTAB128    // For 128 x 128 display
// #define ST7735_GREENTAB160x80 // For 160 x 80 display (BGR, inverted, 26 offset)
// #define ST7735_REDTAB
//#define ST7735_BLACKTAB
// #define ST7735_REDTAB160x80   // For 160 x 80 display with 24 pixel offset

//#define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue
#define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

//#define TFT_INVERSION_ON
//#define TFT_INVERSION_OFF

// Generic ESP32 setup
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
//#define TFT_CS    21 // Not defined here, chip select is managed by sketch
#define TFT_DC    2
#define TFT_RST   4  // Connect reset to ensure display initialises


#define LOAD_FONT2  
#define LOAD_FONT4 


#define SPI_FREQUENCY  27000000
//#define SPI_FREQUENCY  40000000

#define SPI_READ_FREQUENCY  20000000

#define SPI_TOUCH_FREQUENCY  2500000

// #define SUPPORT_TRANSACTIONS