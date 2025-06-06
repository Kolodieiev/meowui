// LilyGo T-Display RP2040 (ST7789 135 x 240 display)
#define USER_SETUP_ID 137

#define ST7789_DRIVER     // Configure all registers

#define TFT_WIDTH  135
#define TFT_HEIGHT 240

#define CGRAM_OFFSET      // Library will add offsets required

// For LilyGo T-Display RP2040
#define TFT_CS   5  // Chip Select pin
#define TFT_DC   1  // Data Command control pin
#define TFT_RST  0  // Reset pin
#define TFT_MOSI 3
#define TFT_SCLK 2
// Don't uncomment next line if you want PWM control of the backlight in the sketch
//#define TFT_BL 4


#define LOAD_FONT2  
#define LOAD_FONT4 

#define SPI_FREQUENCY  40000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000
