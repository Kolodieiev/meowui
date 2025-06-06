// See SetupX_Template.h for all options available
#define USER_SETUP_ID 8

#define ILI9163_DRIVER


#define TFT_WIDTH  128
#define TFT_HEIGHT 128


// For NodeMCU - use pin numbers in the form PIN_Dx where Dx is the NodeMCU pin designation
#define TFT_CS   PIN_D8  // Chip select control pin D8
#define TFT_DC   PIN_D3  // Data Command control pin
#define TFT_RST  PIN_D4  // Reset pin (could connect to NodeMCU RST, see next line)
//#define TFT_RST  -1  // Set TFT_RST to -1 if the display RESET is connected to NodeMCU RST or 3.3V



#define LOAD_FONT2  
#define LOAD_FONT4 


// #define SPI_FREQUENCY  20000000
#define SPI_FREQUENCY  27000000 // Actually sets it to 26.67MHz = 80/3

#define SPI_TOUCH_FREQUENCY  2500000


// #define SUPPORT_TRANSACTIONS
