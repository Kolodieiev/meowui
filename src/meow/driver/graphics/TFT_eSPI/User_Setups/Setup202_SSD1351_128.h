// See SetupX_Template.h for all options available
#define USER_SETUP_ID 202

#define SSD1351_DRIVER


#define TFT_WIDTH  128
#define TFT_HEIGHT 128


#define SSD1351_1DOT5_INCH_128 // For 128 x 128 display

// Wiring:
//   +-------------+------------+-------------------------------------------------------------------+
//   | Display PCB |  TFT_eSPI  |                               Info                                |
//   +-------------+------------+-------------------------------------------------------------------+
//   | GND         | GND (0V)   | Common                                                            |
//   | VCC         | 5V or 3.3V | Better to power with 5V if display PCB supports it                |
//   | DIN         | TFT_MOSI   | SPI data                                                          |
//   | SCK         | TFT_SCLK   | SPI clock                                                         |
//   | DC          | TFT_DC     | Distinguish between a command or its data                         |
//   | RST         | TFT_RST    | Hardware reset, can connect to MCU RST pin as well                |
//   | CS          | TFT_CS     | Chip select, Set to -1 if for manually use with multiple displays |
//   +-------------+------------+-------------------------------------------------------------------+
#if defined(ESP32)
  #define TFT_MOSI 23
  #define TFT_SCLK 18
  #define TFT_DC   2
  #define TFT_RST  4
  #define TFT_CS   15
#elif defined (ARDUINO_ARCH_ESP8266)
//#define TFT_MOSI PIN_D5 // Can't change
//#define TFT_SCLK PIN_D7 // Can't change
  #define TFT_DC   PIN_D3
  #define TFT_RST  PIN_D4
  #define TFT_CS   PIN_D8
#endif



#define LOAD_FONT2  
#define LOAD_FONT4 



#define SPI_FREQUENCY  20000000
//#define SPI_FREQUENCY  40000000 // Works after shielding the wires!
