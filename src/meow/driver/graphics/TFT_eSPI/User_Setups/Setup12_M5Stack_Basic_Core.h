// See SetupX_Template.h for all options available
#define USER_SETUP_ID 12

//Setup file for the M5Stack Basic Core

#define ILI9341_DRIVER

#define M5STACK

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   14  // Chip select control pin
#define TFT_DC   27  // Data Command control pin
#define TFT_RST  33  // Reset pin (could connect to Arduino RESET pin)
#define TFT_BL   32  // LED back-light


#define LOAD_FONT2
#define LOAD_FONT4


#define SPI_FREQUENCY  27000000

// Optional reduced SPI frequency for reading TFT
#define SPI_READ_FREQUENCY  5000000