// Display configuration for ILI9342-based ESP32-S3-Box

#define USER_SETUP_ID 251
#define USER_SETUP_INFO "ESP32-S3-BOX"

#define M5STACK // S3-Box uses the same ILI9342 display, don't remove

#define ILI9341_DRIVER

#define TFT_MISO 0
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   5 // Chip select control pin
#define TFT_DC   4 // Data Command control pin
#define TFT_RST 48 // Reset pin (could connect to Arduino RESET pin)
#define TFT_BL  45 // LED back-light


#define LOAD_FONT2  
#define LOAD_FONT4 



#define SPI_FREQUENCY 27000000
