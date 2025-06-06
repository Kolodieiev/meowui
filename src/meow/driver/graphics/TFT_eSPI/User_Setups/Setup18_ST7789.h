// See SetupX_Template.h for all options available
#define USER_SETUP_ID 18

#define ST7789_DRIVER

// #define TFT_SDA_READ      // This option is for ESP32 ONLY, tested with ST7789 display only

// If colours are inverted (white shows as black) then uncomment one of the next
// 2 lines try both options, one of the options should correct the inversion.
// #define TFT_INVERSION_ON
// #define TFT_INVERSION_OFF

// For ST7789 ONLY, define the colour order IF the blue and red are swapped on your display
// Try ONE option at a time to find the correct colour order for your display
// #define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue
// #define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red


// My ST7789 display has TCT_CS wired permananently low so the pin is not defined here

// For NodeMCU - use pin numbers in the form PIN_Dx where Dx is the NodeMCU pin designation
#define TFT_DC   PIN_D3  // Data Command control pin
#define TFT_RST  PIN_D4  // Reset pin (could connect to NodeMCU RST, see next line)



#define LOAD_FONT2  
#define LOAD_FONT4 


// #define SPI_FREQUENCY  20000000
#define SPI_FREQUENCY  27000000
// #define SPI_FREQUENCY  40000000
// #define SPI_FREQUENCY  80000000

#define SPI_TOUCH_FREQUENCY  2500000


//#define SUPPORT_TRANSACTIONS
