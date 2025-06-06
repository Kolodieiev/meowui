#define USER_SETUP_ID 66

#define GC9A01_DRIVER  // Full configuration option, define additional parameters below for this display

#define TFT_RGB_ORDER TFT_RGB  // Colour order Blue-Green-Red
#define TFT_HEIGHT 240 // GC9A01 240 x 240

#define TFT_SCLK D8
#define TFT_MISO D9
#define TFT_MOSI D10
#define TFT_CS   D1  // Chip select control pin
#define TFT_DC   D3  // Data Command control pin
#define TFT_BL   D6
#define TFT_RST  -1  // Reset pin (could connect to RST pin)


#define LOAD_FONT2  
#define LOAD_FONT4 

#define SPI_FREQUENCY 40000000
#define SPI_READ_FREQUENCY  20000000
// #define USE_HSPI_PORT
