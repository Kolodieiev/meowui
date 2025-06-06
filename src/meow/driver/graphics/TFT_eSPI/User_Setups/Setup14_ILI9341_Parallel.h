// See SetupX_Template.h for all options available
#define USER_SETUP_ID 14

#define TFT_PARALLEL_8_BIT


#define ILI9341_DRIVER


// ESP32 pins used for the parallel interface TFT
#define TFT_CS   33  // Chip select control pin
#define TFT_DC   15  // Data Command control pin - must use a pin in the range 0-31
#define TFT_RST  32  // Reset pin

#define TFT_WR    4  // Write strobe control pin - must use a pin in the range 0-31
#define TFT_RD    2

#define TFT_D0   12  // Must use pins in the range 0-31 for the data bus
#define TFT_D1   13  // so a single register write sets/clears all bits
#define TFT_D2   26
#define TFT_D3   25
#define TFT_D4   17
#define TFT_D5   16
#define TFT_D6   27
#define TFT_D7   14

#define LOAD_FONT2
#define LOAD_FONT4
