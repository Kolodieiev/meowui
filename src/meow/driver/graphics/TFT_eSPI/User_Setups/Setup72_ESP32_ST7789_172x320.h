// Support for 1.47" 320x172 Round Rectangle Color IPS TFT Display
#define USER_SETUP_ID 71

#define ST7789_DRIVER  // Full configuration option, define additional parameters below for this display

#define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

#define TFT_WIDTH 172  // ST7789 172 x 320
#define TFT_HEIGHT 320  // ST7789 240 x 320

#define TFT_BL           21    // LED back-light control pin
#define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)

#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   5   // Chip select control pin
#define TFT_DC   22  // Data Command control pin
#define TFT_RST  17  // Reset pin (could connect to RST pin)


#define LOAD_FONT2  
#define LOAD_FONT4 

#define SPI_FREQUENCY 27000000
