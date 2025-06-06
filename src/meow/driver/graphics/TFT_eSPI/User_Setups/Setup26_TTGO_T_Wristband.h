#define USER_SETUP_ID 26

#define ST7735_DRIVER

#define TFT_WIDTH  80
#define TFT_HEIGHT 160


#define TFT_RST             26   // Set TFT_RST to -1 if the display RESET is connected to NodeMCU RST or 3.3V
#define TFT_MISO            -1
#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              23
#define TFT_BL              27  // Dispaly backlight control pin

#define TFT_BACKLIGHT_ON HIGH  // HIGH or LOW are options

#define ST7735_GREENTAB160x80 // For 160 x 80 display (BGR, inverted, 26 offset)


#define LOAD_FONT2  
#define LOAD_FONT4 

// Comment out the #define below to stop the SPIFFS filing system and smooth font code being loaded
// this will save ~20kbytes of FLASH


#define SPI_FREQUENCY  27000000 // Actually sets it to 26.67MHz = 80/3

// #define SPI_FREQUENCY  40000000 // Maximum to use SPIFFS
// #define SPI_FREQUENCY  80000000
