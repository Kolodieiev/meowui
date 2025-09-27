/***************************************************
  Arduino TFT graphics library targeted at ESP8266
  and ESP32 based boards.

  This is a stand-alone library that contains the
  hardware driver, the graphics functions and the
  proportional fonts.

  The built-in fonts 4, 6, 7 and 8 are Run Length
  Encoded (RLE) to reduce the FLASH footprint.

  Last review/edit by Bodmer: 04/02/22
 ****************************************************/

// Stop fonts etc being loaded multiple times
#pragma once
#pragma GCC optimize("O3")

#define TFT_ESPI_VERSION "2.5.30"

// Bit level feature flags
// Bit 0 set: viewport capability
#define TFT_ESPI_FEATURES 1

/***************************************************************************************
**                         Section 1: Load required header files
***************************************************************************************/

// Standard support
#include <Arduino.h>
#include <Print.h>
#include <SPI.h>
#include <hal/spi_types.h>

/***************************************************************************************
**                         Section 2: Load library and processor specific header files
***************************************************************************************/

#include "./User_Setup_Select.h"

// Include the processor specific drivers
#if defined(CONFIG_IDF_TARGET_ESP32S3)
#include "Processors/TFT_eSPI_ESP32_S3.h"
#elif defined(CONFIG_IDF_TARGET_ESP32)
#include "Processors/TFT_eSPI_ESP32.h"
#else
#pragma GCC error "Бібліотека може працювати тільки на мікроконтролері esp32 або esp32s3"
#endif

/***************************************************************************************
**                         Section 3: Interface setup
***************************************************************************************/
#ifndef TAB_COLOUR
#define TAB_COLOUR 0
#endif

// If the SPI frequency is not defined, set a default
#ifndef SPI_FREQUENCY
#define SPI_FREQUENCY 20000000
#endif

// If the SPI read frequency is not defined, set a default
#ifndef SPI_READ_FREQUENCY
#define SPI_READ_FREQUENCY 10000000
#endif

// Some ST7789 boards do not work with Mode 0
#ifndef TFT_SPI_MODE
#if defined(ST7789_DRIVER) || defined(ST7789_2_DRIVER)
#define TFT_SPI_MODE SPI_MODE3
#else
#define TFT_SPI_MODE SPI_MODE0
#endif
#endif

// If the XPT2046 SPI frequency is not defined, set a default
#ifndef SPI_TOUCH_FREQUENCY
#define SPI_TOUCH_FREQUENCY 2500000
#endif

#ifndef SPI_BUSY_CHECK
#define SPI_BUSY_CHECK
#endif

// If half duplex SDA mode is defined then MISO pin should be -1
#ifdef TFT_SDA_READ
#ifdef TFT_MISO
#if TFT_MISO != -1
#undef TFT_MISO
#define TFT_MISO -1
#warning TFT_MISO set to -1
#endif
#endif
#endif

/***************************************************************************************
**                         Section 4: Setup fonts
***************************************************************************************/

// Only load the fonts defined in User_Setup.h (to save space)
// Set flag so RLE rendering code is optionally compiled
#ifdef LOAD_FONT2
#include "./Fonts/Font2.c"
#endif

#ifdef LOAD_FONT4
#include "./Fonts/Font4.c"
#endif

// Create a null default font in case some fonts not used (to prevent crash)
const uint8_t widtbl_null[1] = {0};
PROGMEM const uint8_t chr_null[1] = {0};
PROGMEM const uint8_t *const chrtbl_null[1] = {chr_null};

// This is a structure to conveniently hold information on the default fonts
// Stores pointer to font character image address table, width table and height
typedef struct
{
  const uint8_t *chartbl;
  const uint8_t *widthtbl;
  uint8_t height;
  uint8_t baseline;
} fontinfo;

// Now fill the structure
const PROGMEM fontinfo fontdata[] = {
    {(const uint8_t *)chrtbl_null, widtbl_null, 0, 0},
    {(const uint8_t *)chrtbl_null, widtbl_null, 8, 7},
#ifdef LOAD_FONT2
    {(const uint8_t *)chrtbl_f2, widtbl_f2, chr_hgt_font2, baseline_f2},
#else
    {(const uint8_t *)chrtbl_null, widtbl_null, 0, 0},
#endif
    {(const uint8_t *)chrtbl_null, widtbl_null, 0, 0},
#ifdef LOAD_FONT4
    {(const uint8_t *)chrtbl_f4, widtbl_f4, chr_hgt_font4, baseline_f4},
#else
    {(const uint8_t *)chrtbl_null, widtbl_null, 0, 0},
#endif
};

/***************************************************************************************
**                         Section 5: Font datum enumeration
***************************************************************************************/
// These enumerate the text plotting alignment (reference datum point)
#define TL_DATUM 0    // Top left (default)
#define TC_DATUM 1    // Top centre
#define TR_DATUM 2    // Top right
#define ML_DATUM 3    // Middle left
#define CL_DATUM 3    // Centre left, same as above
#define MC_DATUM 4    // Middle centre
#define CC_DATUM 4    // Centre centre, same as above
#define MR_DATUM 5    // Middle right
#define CR_DATUM 5    // Centre right, same as above
#define BL_DATUM 6    // Bottom left
#define BC_DATUM 7    // Bottom centre
#define BR_DATUM 8    // Bottom right
#define L_BASELINE 9  // Left character baseline (Line the 'A' character would sit on)
#define C_BASELINE 10 // Centre character baseline
#define R_BASELINE 11 // Right character baseline

/***************************************************************************************
**                         Section 6: Colour enumeration
***************************************************************************************/
// Default color definitions
#define TFT_BLACK 0x0000     /*   0,   0,   0 */
#define TFT_NAVY 0x000F      /*   0,   0, 128 */
#define TFT_DARKGREEN 0x03E0 /*   0, 128,   0 */
#define TFT_DARKCYAN 0x03EF  /*   0, 128, 128 */
#define TFT_MAROON 0x7800    /* 128,   0,   0 */
#define TFT_PURPLE 0x780F    /* 128,   0, 128 */
#define TFT_OLIVE 0x7BE0     /* 128, 128,   0 */
#define TFT_LIGHTGREY 0xD69A /* 211, 211, 211 */
#define TFT_LIME 0xA7E0
#define TFT_DARKGREY 0x3186
#define TFT_BLUE 0x001F                     /*   0,   0, 255 */
#define TFT_GREEN 0x07E0                    /*   0, 255,   0 */
#define TFT_CYAN 0x07FF                     /*   0, 255, 255 */
#define TFT_RED 0xF800                      /* 255,   0,   0 */
#define TFT_MAGENTA 0xF81F                  /* 255,   0, 255 */
#define TFT_YELLOW 0xFFE0                   /* 255, 255,   0 */
#define TFT_WHITE 0xFFFF                    /* 255, 255, 255 */
#define TFT_ORANGE 0xFDA0                   /* 255, 180,   0 */
#define TFT_GREENYELLOW 0xB7E0              /* 180, 255,   0 */
#define TFT_PINK 0xFE19 /* 255, 192, 203 */ // Lighter pink, was 0xFC9F
#define TFT_BROWN 0x9A60                    /* 150,  75,   0 */
#define TFT_GOLD 0xFEA0                     /* 255, 215,   0 */
#define TFT_SILVER 0xC618                   /* 192, 192, 192 */
#define TFT_SKYBLUE 0x867D                  /* 135, 206, 235 */
#define TFT_VIOLET 0x915C                   /* 180,  46, 226 */

#define TFT_TRANSPARENT 0xF81F // Колір rgb(255, 0, 255)

// Default palette for 4 bit colour sprites
const uint16_t default_4bit_palette[] PROGMEM = {
    TFT_BLACK,     //  0  ^
    TFT_BROWN,     //  1  |
    TFT_RED,       //  2  |
    TFT_ORANGE,    //  3  |
    TFT_YELLOW,    //  4  Colours 0-9 follow the resistor colour code!
    TFT_GREEN,     //  5  |
    TFT_BLUE,      //  6  |
    TFT_PURPLE,    //  7  |
    TFT_DARKGREY,  //  8  |
    TFT_WHITE,     //  9  v
    TFT_CYAN,      // 10  Blue+green mix
    TFT_MAGENTA,   // 11  Blue+red mix
    TFT_MAROON,    // 12  Darker red colour
    TFT_DARKGREEN, // 13  Darker green colour
    TFT_NAVY,      // 14  Darker blue colour
    TFT_PINK       // 15
};

/***************************************************************************************
**                         Section 7: Diagnostic support
***************************************************************************************/
// #define TFT_eSPI_DEBUG     // Switch on debug support serial messages  (not used yet)
// #define TFT_eSPI_FNx_DEBUG // Switch on debug support for function "x" (not used yet)

// Clipping macro for pushImage
#define PI_CLIP                   \
  if (_vpOoB)                     \
    return;                       \
  x += _xDatum;                   \
  y += _yDatum;                   \
                                  \
  if ((x >= _vpW) || (y >= _vpH)) \
    return;                       \
                                  \
  int32_t dx = 0;                 \
  int32_t dy = 0;                 \
  int32_t dw = w;                 \
  int32_t dh = h;                 \
                                  \
  if (x < _vpX)                   \
  {                               \
    dx = _vpX - x;                \
    dw -= dx;                     \
    x = _vpX;                     \
  }                               \
  if (y < _vpY)                   \
  {                               \
    dy = _vpY - y;                \
    dh -= dy;                     \
    y = _vpY;                     \
  }                               \
                                  \
  if ((x + dw) > _vpW)            \
    dw = _vpW - x;                \
  if ((y + dh) > _vpH)            \
    dh = _vpH - y;                \
                                  \
  if (dw < 1 || dh < 1)           \
    return;

/***************************************************************************************
**                         Section 8: Class member and support functions
***************************************************************************************/

// Callback prototype for smooth font pixel colour read
typedef uint16_t (*getColorCallback)(uint16_t x, uint16_t y);

// Class functions and variables
class TFT_eSPI
{
  friend class TFT_eSprite; // Sprite class has access to protected members

  //--------------------------------------- public ------------------------------------//
public:
  explicit TFT_eSPI(int16_t _W = TFT_WIDTH, int16_t _H = TFT_HEIGHT);

  // init() and begin() are equivalent, begin() included for backwards compatibility
  // Sketch defined tab colour option is for ST7735 displays only
  void init(uint8_t tc = TAB_COLOUR), begin(uint8_t tc = TAB_COLOUR);

  // These are virtual so the TFT_eSprite class can override them with sprite specific functions
  virtual void drawPixel(int32_t x, int32_t y, uint32_t color),
      drawChar(int32_t x, int32_t y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size),
      drawLine(int32_t xs, int32_t ys, int32_t xe, int32_t ye, uint32_t color),
      drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color),
      drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color),
      fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);

  virtual int16_t drawChar(uint16_t uniCode, int32_t x, int32_t y, uint8_t font);
  virtual int16_t drawChar(uint16_t uniCode, int32_t x, int32_t y);

  virtual int16_t height(void);
  virtual int16_t width(void);

  // Read the colour of a pixel at x,y and return value in 565 format
  virtual uint16_t readPixel(int32_t x, int32_t y);

  virtual void setWindow(int32_t xs, int32_t ys, int32_t xe, int32_t ye); // Note: start + end coordinates

  // Push (aka write pixel) colours to the set window
  virtual void pushColor(uint16_t color);

  // These are non-inlined to enable override
  virtual void begin_nin_write();
  virtual void end_nin_write();

  void setRotation(uint8_t r); // Set the display image orientation to 0, 1, 2 or 3
  uint8_t getRotation(void);   // Read the current rotation

  // Change the origin position from the default top left
  // Note: setRotation, setViewport and resetViewport will revert origin to top left corner of screen/sprite
  void setOrigin(int32_t x, int32_t y);
  int32_t getOriginX(void);
  int32_t getOriginY(void);

  void invertDisplay(bool i); // Tell TFT to invert all displayed colours

  // The TFT_eSprite class inherits the following functions (not all are useful to Sprite class
  void setAddrWindow(int32_t xs, int32_t ys, int32_t w, int32_t h); // Note: start coordinates + width and height

  // Viewport commands, see "Viewport_Demo" sketch
  void setViewport(int32_t x, int32_t y, int32_t w, int32_t h, bool vpDatum = true);
  bool checkViewport(int32_t x, int32_t y, int32_t w, int32_t h);
  int32_t getViewportX(void);
  int32_t getViewportY(void);
  int32_t getViewportWidth(void);
  int32_t getViewportHeight(void);
  bool getViewportDatum(void);
  void frameViewport(uint16_t color, int32_t w);
  void resetViewport(void);

  // Clip input window to viewport bounds, return false if whole area is out of bounds
  bool clipAddrWindow(int32_t *x, int32_t *y, int32_t *w, int32_t *h);
  // Clip input window area to viewport bounds, return false if whole area is out of bounds
  bool clipWindow(int32_t *xs, int32_t *ys, int32_t *xe, int32_t *ye);

  // Push (aka write pixel) colours to the TFT (use setAddrWindow() first)
  void pushColor(uint16_t color, uint32_t len),                   // Deprecated, use pushBlock()
      pushColors(uint16_t *data, uint32_t len, bool swap = true), // With byte swap option
      pushColors(uint8_t *data, uint32_t len);                    // Deprecated, use pushPixels()

  // Write a solid block of a single colour
  void pushBlock(uint16_t color, uint32_t len);

  // Write a set of pixels stored in memory, use setSwapBytes(true/false) function to correct endianess
  void pushPixels(const void *data_in, uint32_t len);

// Support for half duplex (bi-directional SDA) SPI bus where MOSI must be switched to input
#ifdef TFT_SDA_READ
#if defined(TFT_eSPI_ENABLE_8_BIT_READ)
  uint8_t tft_Read_8(void); // Read 8 bit value from TFT command register
#endif
  void begin_SDA_Read(void); // Begin a read on a half duplex (bi-directional SDA) SPI bus - sets MOSI to input
  void end_SDA_Read(void);   // Restore MOSI to output
#endif

  // Graphics drawing
  void fillScreen(uint32_t color),
      drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color),
      drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint32_t color),
      fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint32_t color);

  void fillRectVGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2);
  void fillRectHGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2);

  void drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color),
      drawCircleHelper(int32_t x, int32_t y, int32_t r, uint8_t cornername, uint32_t color),
      fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color),
      fillCircleHelper(int32_t x, int32_t y, int32_t r, uint8_t cornername, int32_t delta, uint32_t color),

      drawEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color),
      fillEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color),

      //                 Corner 1               Corner 2               Corner 3
      drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color),
      fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);

  // Smooth (anti-aliased) graphics drawing
  // Draw a pixel blended with the background pixel colour (bg_color) specified,  return blended colour
  // If the bg_color is not specified, the background pixel colour will be read from TFT or sprite
  uint16_t drawPixel(int32_t x, int32_t y, uint32_t color, uint8_t alpha, uint32_t bg_color = 0x00FFFFFF);

  // Draw an anti-aliased (smooth) arc between start and end angles. Arc ends are anti-aliased.
  // By default the arc is drawn with square ends unless the "roundEnds" parameter is included and set true
  // Angle = 0 is at 6 o'clock position, 90 at 9 o'clock etc. The angles must be in range 0-360 or they will be clipped to these limits
  // The start angle may be larger than the end angle. Arcs are always drawn clockwise from the start angle.
  void drawSmoothArc(int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle, uint32_t fg_color, uint32_t bg_color, bool roundEnds = false);

  // As per "drawSmoothArc" except the ends of the arc are NOT anti-aliased, this facilitates dynamic arc length changes with
  // arc segments and ensures clean segment joints.
  // The sides of the arc are anti-aliased by default. If smoothArc is false sides will NOT be anti-aliased
  void drawArc(int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle, uint32_t fg_color, uint32_t bg_color, bool smoothArc = true);

  // Draw an anti-aliased filled circle at x, y with radius r
  // Note: The thickness of line is 3 pixels to reduce the visible "braiding" effect of anti-aliasing narrow lines
  //       this means the inner anti-alias zone is always at r-1 and the outer zone at r+1
  void drawSmoothCircle(int32_t x, int32_t y, int32_t r, uint32_t fg_color, uint32_t bg_color);

  // Draw an anti-aliased filled circle at x, y with radius r
  // If bg_color is not included the background pixel colour will be read from TFT or sprite
  void fillSmoothCircle(int32_t x, int32_t y, int32_t r, uint32_t color, uint32_t bg_color = 0x00FFFFFF);

  // Draw a rounded rectangle that has a line thickness of r-ir+1 and bounding box defined by x,y and w,h
  // The outer corner radius is r, inner corner radius is ir
  // The inside and outside of the border are anti-aliased
  void drawSmoothRoundRect(int32_t x, int32_t y, int32_t r, int32_t ir, int32_t w, int32_t h, uint32_t fg_color, uint32_t bg_color = 0x00FFFFFF, uint8_t quadrants = 0xF);

  // Draw a filled rounded rectangle , corner radius r and bounding box defined by x,y and w,h
  void fillSmoothRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint32_t color, uint32_t bg_color = 0x00FFFFFF);

  // Draw a small anti-aliased filled circle at ax,ay with radius r (uses drawWideLine)
  // If bg_color is not included the background pixel colour will be read from TFT or sprite
  void drawSpot(float ax, float ay, float r, uint32_t fg_color, uint32_t bg_color = 0x00FFFFFF);

  // Draw an anti-aliased wide line from ax,ay to bx,by width wd with radiused ends (radius is wd/2)
  // If bg_color is not included the background pixel colour will be read from TFT or sprite
  void drawWideLine(float ax, float ay, float bx, float by, float wd, uint32_t fg_color, uint32_t bg_color = 0x00FFFFFF);

  // Draw an anti-aliased wide line from ax,ay to bx,by with different width at each end aw, bw and with radiused ends
  // If bg_color is not included the background pixel colour will be read from TFT or sprite
  void drawWedgeLine(float ax, float ay, float bx, float by, float aw, float bw, uint32_t fg_color, uint32_t bg_color = 0x00FFFFFF);

  // Image rendering
  // Swap the byte order for pushImage() and pushPixels() - corrects endianness
  void setSwapBytes(bool swap);
  bool getSwapBytes(void);

  // Draw bitmap
  void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor),
      drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor, uint16_t bgcolor),
      drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor),
      drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor, uint16_t bgcolor),
      setBitmapColor(uint16_t fgcolor, uint16_t bgcolor); // Define the 2 colours for 1bpp sprites

  // Set TFT pivot point (use when rendering rotated sprites)
  void setPivot(int16_t x, int16_t y);
  int16_t getPivotX(void), // Get pivot x
      getPivotY(void);     // Get pivot y

  // The next functions can be used as a pair to copy screen blocks (or horizontal/vertical lines) to another location
  // Read a block of pixels to a data buffer, buffer is 16 bit and the size must be at least w * h
  void readRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data);
  // Write a block of pixels to the screen which have been read by readRect()
  void pushRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data);

  // These are used to render images or sprites stored in RAM arrays (used by Sprite class for 16bpp Sprites)
  void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data);
  void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data, uint16_t transparent);

  // These are used to render images stored in FLASH (PROGMEM)
  void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data, uint16_t transparent);
  void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data);

  // These are used by Sprite class pushSprite() member function for 1, 4 and 8 bits per pixel (bpp) colours
  // They are not intended to be used with user sketches (but could be)
  // Set bpp8 true for 8bpp sprites, false otherwise. The cmap pointer must be specified for 4bpp
  void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data, bool bpp8 = true, uint16_t *cmap = nullptr);
  void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data, uint8_t transparent, bool bpp8 = true, uint16_t *cmap = nullptr);
  // FLASH version
  void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint8_t *data, bool bpp8, uint16_t *cmap = nullptr);

  // Render a 16 bit colour image with a 1bpp mask
  void pushMaskedImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *img, uint8_t *mask);

  // This next function has been used successfully to dump the TFT screen to a PC for documentation purposes
  // It reads a screen area and returns the 3 RGB 8 bit colour values of each pixel in the buffer
  // Set w and h to 1 to read 1 pixel's colour. The data buffer must be at least w * h * 3 bytes
  void readRectRGB(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data);

  // Text rendering - value returned is the pixel width of the rendered text
  int16_t drawNumber(long intNumber, int32_t x, int32_t y, uint8_t font), // Draw integer using specified font number
      drawNumber(long intNumber, int32_t x, int32_t y),                   // Draw integer using current font

      // Decimal is the number of decimal places to render
      // Use with setTextDatum() to position values on TFT, and setTextPadding() to blank old displayed values
      drawFloat(float floatNumber, uint8_t decimal, int32_t x, int32_t y, uint8_t font), // Draw float using specified font number
      drawFloat(float floatNumber, uint8_t decimal, int32_t x, int32_t y),               // Draw float using current font

      // Handle char arrays
      // Use with setTextDatum() to position string on TFT, and setTextPadding() to blank old displayed strings
      drawString(const char *string, int32_t x, int32_t y, uint8_t font),   // Draw string using specified font number
      drawString(const char *string, int32_t x, int32_t y),                 // Draw string using current font
      drawString(const String &string, int32_t x, int32_t y, uint8_t font), // Draw string using specified font number
      drawString(const String &string, int32_t x, int32_t y),               // Draw string using current font

      drawCentreString(const char *string, int32_t x, int32_t y, uint8_t font),   // Deprecated, use setTextDatum() and drawString()
      drawRightString(const char *string, int32_t x, int32_t y, uint8_t font),    // Deprecated, use setTextDatum() and drawString()
      drawCentreString(const String &string, int32_t x, int32_t y, uint8_t font), // Deprecated, use setTextDatum() and drawString()
      drawRightString(const String &string, int32_t x, int32_t y, uint8_t font);  // Deprecated, use setTextDatum() and drawString()

  // Text rendering and font handling support funtions
  void setCursor(int16_t x, int16_t y),              // Set cursor for tft.print()
      setCursor(int16_t x, int16_t y, uint8_t font); // Set cursor and font number for tft.print()

  int16_t getCursorX(void), // Read current cursor x position (moves with tft.print())
      getCursorY(void);     // Read current cursor y position

  void setTextColor(uint16_t color),                                         // Set character (glyph) color only (background not over-written)
      setTextColor(uint16_t fgcolor, uint16_t bgcolor, bool bgfill = false), // Set character (glyph) foreground and background colour, optional background fill for smooth fonts
      setTextSize(uint8_t size);                                             // Set character size multiplier (this increases pixel size)

  void setTextWrap(bool wrapX, bool wrapY = false); // Turn on/off wrapping of text in TFT width and/or height

  void setTextDatum(uint8_t datum); // Set text datum position (default is top left), see Section 6 above
  uint8_t getTextDatum(void);

  void setTextPadding(uint16_t x_width); // Set text padding (background blanking/over-write) width in pixels
  uint16_t getTextPadding(void);         // Get text padding

  void setFreeFont(uint8_t font), // Not used, historical fix to prevent an error
      setTextFont(uint8_t font);  // Set the font number to use in future

  int16_t textWidth(const char *string, uint8_t font), // Returns pixel width of string in specified font
      textWidth(const char *string),                   // Returns pixel width of string in current font
      textWidth(const String &string, uint8_t font),   // As above for String types
      textWidth(const String &string),
      fontHeight(int16_t font), // Returns pixel height of string in specified font
      fontHeight(void);         // Returns pixel width of string in current font

  // Used by library and Smooth font class to extract Unicode point codes from a UTF8 encoded string
  uint32_t decodeUTF8(const uint8_t *buf, uint16_t &byte_pos, uint16_t remaining);

  // Used by Smooth font class to fetch a pixel colour for the anti-aliasing
  void setCallback(getColorCallback getCol);

  uint16_t fontsLoaded(void); // Each bit in returned value represents a font type that is loaded - used for debug/error handling only

  // Low level read/write
  void spiwrite(uint8_t); // legacy support only
#ifdef RM68120_DRIVER
  void writecommand(uint16_t c);                // Send a 16 bit command, function resets DC/RS high ready for data
  void writeRegister8(uint16_t c, uint8_t d);   // Write 8 bit data data to 16 bit command register
  void writeRegister16(uint16_t c, uint16_t d); // Write 16 bit data data to 16 bit command register
#else
  void writecommand(uint8_t c); // Send an 8 bit command, function resets DC/RS high ready for data
#endif
  void writedata(uint8_t d); // Send data with DC/RS set high

  void commandList(const uint8_t *addr); // Send a initialisation sequence to TFT stored in FLASH

  uint8_t readcommand8(uint8_t cmd_function, uint8_t index = 0);   // read 8 bits from TFT
  uint16_t readcommand16(uint8_t cmd_function, uint8_t index = 0); // read 16 bits from TFT
  uint32_t readcommand32(uint8_t cmd_function, uint8_t index = 0); // read 32 bits from TFT

  // Colour conversion
  // Convert 8 bit red, green and blue to 16 bits
  uint16_t color565(uint8_t red, uint8_t green, uint8_t blue);

  // Convert 8 bit colour to 16 bits
  uint16_t color8to16(uint8_t color332);
  // Convert 16 bit colour to 8 bits
  uint8_t color16to8(uint16_t color565);

  // Convert 16 bit colour to/from 24 bit, R+G+B concatenated into LS 24 bits
  uint32_t color16to24(uint16_t color565);
  uint32_t color24to16(uint32_t color888);

  // Alpha blend 2 colours, see generic "alphaBlend_Test" example
  // alpha =   0 = 100% background colour
  // alpha = 255 = 100% foreground colour
  uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc);
  // 16 bit colour alphaBlend with alpha dither (dither reduces colour banding)
  uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc, uint8_t dither);
  // 24 bit colour alphaBlend with optional alpha dither
  uint32_t alphaBlend24(uint8_t alpha, uint32_t fgc, uint32_t bgc, uint8_t dither = 0);

  // Direct Memory Access (DMA) support functions
  // These can be used for SPI writes when using the ESP32 (original) or STM32 processors.
  // DMA also works on a RP2040 processor with PIO based SPI and parallel (8 and 16 bit) interfaces
  // Bear in mind DMA will only be of benefit in particular circumstances and can be tricky
  // to manage by noobs. The functions have however been designed to be noob friendly and
  // avoid a few DMA behaviour "gotchas".
  //
  // At best you will get a 2x TFT rendering performance improvement when using DMA because
  // this library handles the SPI bus so efficiently during normal (non DMA) transfers. The best
  // performance improvement scenario is the DMA transfer time is exactly the same as the time it
  // takes for the processor to prepare the next image buffer and initiate another DMA transfer.
  //
  // DMA transfer to the TFT is done while the processor moves on to handle other tasks. Bear
  // this in mind and watch out for "gotchas" like the image buffer going out of scope as the
  // processor leaves a function or its content being changed while the DMA engine is reading it.
  //
  // The compiler MAY change the implied scope of a buffer which has been set aside by creating
  // an array. For example a buffer defined before a "for-next" loop may get de-allocated when
  // the loop ends. To avoid this use, for example, malloc() and free() to take control of when
  // the buffer space is available and ensure it is not released until DMA is complete.
  //
  // Clearly you should not modify a buffer that is being DMA'ed to the TFT until the DMA is over.
  // Use the dmaBusy() function to check this.  Use tft.startWrite() before invoking DMA so the
  // TFT chip select stays low. If you use tft.endWrite() before DMA is complete then the endWrite
  // function will wait for the DMA to complete, so this may defeat any DMA performance benefit.
  //

  bool initDMA(bool ctrl_cs = false); // Initialise the DMA engine and attach to SPI bus - typically used in setup()
                                      // Parameter "true" enables DMA engine control of TFT chip select (ESP32 only)
                                      // For ESP32 only, TFT reads will not work if parameter is true
  void deInitDMA(void);               // De-initialise the DMA engine and detach from SPI bus - typically not used

  // Push an image to the TFT using DMA, buffer is optional and grabs (double buffers) a copy of the image
  // Use the buffer if the image data will get over-written or destroyed while DMA is in progress
  //
  // Note 1: If swapping colour bytes is defined, and the double buffer option is NOT used, then the bytes
  // in the original image buffer content will be byte swapped by the function before DMA is initiated.
  //
  // Note 2: If part of the image will be off screen or outside of a set viewport, then the the original
  // image buffer content will be altered to a correctly clipped image before DMA is initiated.
  //
  // The function will wait for the last DMA to complete if it is called while a previous DMA is still
  // in progress, this simplifies the sketch and helps avoid "gotchas".
  void pushImageDMA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data, uint16_t *buffer = nullptr);

#if defined(ESP32) // ESP32 only at the moment
                   // For case where pointer is a const and the image data must not be modified (clipped or byte swapped)
  void pushImageDMA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t const *data);
#endif
  // Push a block of pixels into a window set up using setAddrWindow()
  void pushPixelsDMA(uint16_t *image, uint32_t len);

  // Check if the DMA is complete - use while(tft.dmaBusy); for a blocking wait
  bool dmaBusy(void); // returns true if DMA is still in progress
  void dmaWait(void); // wait until DMA is complete

  bool DMA_Enabled = false; // Flag for DMA enabled state
  uint8_t spiBusyCheck = 0; // Number of ESP32 transfer buffers to check

  // Bare metal functions
  void startWrite(void);                         // Begin SPI transaction
  void writeColor(uint16_t color, uint32_t len); // Deprecated, use pushBlock()
  void endWrite(void);                           // End SPI transaction

  // Set/get an arbitrary library configuration attribute or option
  //       Use to switch ON/OFF capabilities such as UTF8 decoding - each attribute has a unique ID
  //       id = 0: reserved - may be used in future to reset all attributes to a default state
  //       id = 1: Turn on (a=true) or off (a=false) GLCD cp437 font character error correction
  //       id = 2: Turn on (a=true) or off (a=false) UTF8 decoding
  //       id = 3: Enable or disable use of ESP32 PSRAM (if available)
#define CP437_SWITCH 1
#define UTF8_SWITCH 2
#define PSRAM_ENABLE 3
  void setAttribute(uint8_t id = 0, uint8_t a = 0); // Set attribute value
  uint8_t getAttribute(uint8_t id = 0);             // Get attribute value

  // Global variables
  static SPIClass &getSPIinstance(void); // Get SPI class handle
  //--------------------------------------- private ------------------------------------//

private:
#if defined(ESP32_PARALLEL)
  // Lookup table for ESP32 parallel bus interface uses 1kbyte RAM,
  uint32_t xset_mask[256]; // Makes Sprite rendering test 33% faster, for slower macro equivalent
                           // see commented out #define set_mask(C) within TFT_eSPI_ESP32.h
#endif
  // Port and pin masks for control signals (ESP826 only) - TODO: remove need for this
  volatile uint32_t *dcport, *csport;
  getColorCallback getColor = nullptr; // Smooth font callback function pointer

private:
  uint32_t textcolor, textbgcolor; // Text foreground and background colours
  uint32_t bitmap_fg, bitmap_bg;   // Bitmap foreground (bit=1) and background (bit=0) colours
  uint32_t cspinmask, dcpinmask, wrpinmask, sclkpinmask;

#if defined(ESP32_PARALLEL)
  // Bit masks for ESP32 parallel bus interface
  uint32_t xclr_mask, xdir_mask; // Port set/clear and direction control masks
#endif

protected:
  int32_t _init_width, _init_height; // Display w/h as input, used by setRotation()
  int32_t _width, _height;           // Display w/h as modified by current rotation
  int32_t addr_row, addr_col;        // Window position - used to minimise window commands

  // Viewport variables
  int32_t _vpX, _vpY, _vpW, _vpH; // Note: x start, y start, x end + 1, y end + 1
  int32_t _xDatum;
  int32_t _yDatum;
  int32_t _xWidth;
  int32_t _yHeight;

  int32_t cursor_x, cursor_y, padX; // Text cursor x,y and padding setting
  int32_t bg_cursor_x;              // Background fill cursor
  int32_t last_cursor_x;            // Previous text cursor position when fill used

  uint32_t fontsloaded; // Bit field of fonts loaded
  uint32_t _lastColor;  // Buffered value of last colour used

private:
  uint16_t decoderBuffer; // Unicode code-point buffer - not for user access

#if defined(SSD1963_DRIVER)
  uint16_t Cswap; // Swap buffer for SSD1963
#endif

protected:
  int16_t _xPivot; // TFT x pivot point coordinate for rotated Sprites
  int16_t _yPivot; // TFT x pivot point coordinate for rotated Sprites

private:
  uint8_t textfont, // Current selected font number
      textsize,     // Current font size multiplier
      textdatum,    // Text reference datum
      rotation;     // Display rotation (0-3)

  uint8_t decoderState = 0; // UTF8 decoder state - not for user access
  // Display variant settings
  uint8_t tabcolor = 0,           // ST7735 screen protector "tab" colour (now invalid)
      colstart = 0, rowstart = 0; // Screen display area to CGRAM area coordinate offsets

#if defined(SSD1963_DRIVER)
  uint8_t r6, g6, b6; // RGB buffer for SSD1963
#endif

private:
  bool locked, inTransaction, lockTransaction; // SPI transaction and mutex lock flags

protected:
  bool _vpDatum;
  bool _vpOoB;
  bool isDigits;             // adjust bounding box for numbers to reduce visual jiggling
  bool textwrapX, textwrapY; // If set, 'wrap' text at right and optionally bottom edge of display
  bool _swapBytes;           // Swap the byte order for TFT pushImage()
  bool _booted;              // init() or begin() has already run once
  // User sketch manages these via set/getAttribute()
  bool _cp437;        // If set, use correct CP437 charset (default is ON)
  bool _utf8;         // If set, use UTF-8 decoder in print stream 'write()' function (default ON)
  bool _psram_enable; // Enable PSRAM use for library functions (TBD) and Sprites
  bool _fillbg;       // Fill background flag (just for for smooth fonts at the moment)

private:
  // Legacy begin and end prototypes - deprecated TODO: delete
  void spi_begin();
  void spi_end();

  void spi_begin_read();
  void spi_end_read();

  // New begin and end prototypes
  // begin/end a TFT write transaction
  // For SPI bus the transmit clock rate is set
  inline void begin_tft_write() __attribute__((always_inline));
  inline void end_tft_write() __attribute__((always_inline));

  // begin/end a TFT read transaction
  // For SPI bus: begin lowers SPI clock rate, end reinstates transmit clock rate
  inline void begin_tft_read() __attribute__((always_inline));
  inline void end_tft_read() __attribute__((always_inline));

  // Initialise the data bus GPIO and hardware interfaces
  void initBus(void);

  // Temporary  library development function  TODO: remove need for this
  void pushSwapBytePixels(const void *data_in, uint32_t len);

  // Same as setAddrWindow but exits with CGRAM in read mode
  void readAddrWindow(int32_t xs, int32_t ys, int32_t w, int32_t h);

  // Byte read prototype
  uint8_t readByte(void);

  // GPIO parallel bus input/output direction control
  void busDir(uint32_t mask, uint8_t mode);

  // Single GPIO input/output direction control
  void gpioMode(uint8_t gpio, uint8_t mode);

  // Smooth graphics helper
  uint8_t sqrt_fraction(uint32_t num);

  // Helper function: calculate distance of a point from a finite length line between two points
  float wedgeLineDistance(float pax, float pay, float bax, float bay, float dr);

  /***************************************************************************************
  **                         Section 9: TFT_eSPI class conditional extensions
  ***************************************************************************************/
}; // End of class TFT_eSPI

// Swap any type
template <typename T>
static inline void
transpose(T &a, T &b)
{
  T t = a;
  a = b;
  b = t;
}

/***************************************************************************************
**                         Section 10: Additional extension classes
***************************************************************************************/
// Load the Sprite Class
#include "./TFT_eSprite.h"
