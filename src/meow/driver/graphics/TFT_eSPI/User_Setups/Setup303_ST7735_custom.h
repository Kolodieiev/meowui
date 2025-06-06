#define DISABLE_ALL_LIBRARY_WARNINGS // Вимкнути попередження бібліотеки.
#define USER_SETUP_ID 303            // Ідентифікатор налаштувань.
#define ST7735_DRIVER                // Назва драйверу (Не міняти!)
#define ST7735_GREENTAB160x80        // Костиль для дисплея 160х80. For 160 x 80 display (BGR, inverted, 26 offset)
#define SPI_FREQUENCY 40000000
//---------------------------------------------------
#define TFT_WIDTH 80
#define TFT_HEIGHT 160
//---------------------------------------------------
#define LOAD_FONT2
#define LOAD_FONT4
//---------------------------------------------------

// esp8266
// #define TFT_MOSI 13
// #define TFT_SCLK 14
// #define TFT_CS   4
// #define TFT_DC   5
// #define TFT_RST  16

//---------------------------------------------------

// esp32
#define TFT_MOSI 23 // sda
#define TFT_SCLK 18 // scl
#define TFT_CS 15
#define TFT_DC 2

#define TFT_RST  -1 // Якщо скидання дисплею під'єднано до піну апаратного скидання esp.
// #define TFT_RST 4
