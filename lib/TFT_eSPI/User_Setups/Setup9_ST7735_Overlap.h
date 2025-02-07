// See SetupX_Template.h for all options available
#define USER_SETUP_ID 9

#define ST7735_DRIVER

#define TFT_WIDTH 128
#define TFT_HEIGHT 160

#define ST7735_REDTAB

// For ST7735, ST7789 and ILI9341 ONLY, define the colour order IF the blue and red are swapped on your display
// Try ONE option at a time to find the correct colour order for your display

//  #define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue
//  #define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

// Overlap mode shares the ESP8266 FLASH SPI bus with the TFT so has a performance impact
// but saves pins for other functions. It is best not to connect MISO as some displays
// do not tristate that line when chip select is high!
// Note: Only one SPI device can share the FLASH SPI lines, so a SPI touch controller
// cannot be connected as well to the same SPI signals.
// On NodeMCU 1.0 SD0=MISO, SD1=MOSI, CLK=SCLK to connect to TFT in overlap mode
// On NodeMCU V3  S0 =MISO, S1 =MOSI, S2 =SCLK
// In ESP8266 overlap mode the following must be defined
#define TFT_SPI_OVERLAP

// In ESP8266 overlap mode the TFT chip select MUST connect to pin D3
#define TFT_CS PIN_D3
#define TFT_DC PIN_D5  // Data Command control pin
#define TFT_RST PIN_D4 // Reset pin (could connect to NodeMCU RST, see next line)
// #define TFT_RST  -1  // Set TFT_RST to -1 if the display RESET is connected to NodeMCU RST or 3.3V

#define LOAD_FONT2
#define LOAD_FONT4

// #define SPI_FREQUENCY  20000000
#define SPI_FREQUENCY 27000000

#define SPI_TOUCH_FREQUENCY 2500000

// #define SUPPORT_TRANSACTIONS
