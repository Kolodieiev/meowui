// See SetupX_Template.h for all options available
#define USER_SETUP_ID 200

#define GC9A01_DRIVER

// For ESP32 Dev board (only tested with GC9A01 display)
// The hardware SPI can be mapped to any pins

#define TFT_MOSI 15 // In some display driver board, it might be written as "SDA" and so on.
#define TFT_SCLK 14
#define TFT_CS   5  // Chip select control pin
#define TFT_DC   27  // Data Command control pin
#define TFT_RST  33  // Reset pin (could connect to Arduino RESET pin)
#define TFT_BL   22  // LED back-light



#define LOAD_FONT2  
#define LOAD_FONT4 


//#define SPI_FREQUENCY  80000000
#define SPI_FREQUENCY  27000000

// Optional reduced SPI frequency for reading TFT
#define SPI_READ_FREQUENCY  5000000