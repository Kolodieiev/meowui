/***************************************************************************************
// The following class creates Sprites in RAM, graphics can then be drawn in the Sprite
// and rendered quickly onto the TFT screen. The class inherits the graphics functions
// from the TFT_eSPI class. Some functions are overridden by this class so that the
// graphics are written to the Sprite rather than the TFT.
***************************************************************************************/
#pragma GCC optimize("O3")
#pragma once
#include "./TFT_eSPI.h"

// cppcheck-suppress duplInheritedMember
class TFT_eSprite : public TFT_eSPI
{

public:
  explicit TFT_eSprite(TFT_eSPI *tft);
  ~TFT_eSprite(void);

  // Міняє місцями буфери малювання спрайтів.
  void swapBuffs16(TFT_eSprite &other);
  // Міняє місцями буфери малювання спрайтів.
  void swapBuffs8(TFT_eSprite &other);

  // Швидке копіювання буферів малювання.
  void fastCopy16(TFT_eSprite &to);
  // Швидке копіювання буферів малювання.
  void fastCopy8(TFT_eSprite &to);

  // Create a sprite of width x height pixels, return a pointer to the RAM area
  // Sketch can cast returned value to (uint16_t*) for 16 bit depth if needed
  // RAM required is:
  //  - 1 bit per pixel for 1 bit colour depth
  //  - 1 nibble per pixel for 4 bit colour (with palette table)
  //  - 1 byte per pixel for 8 bit colour (332 RGB format)
  //  - 2 bytes per pixel for 16 bit color depth (565 RGB format)
  void *createSprite(int16_t width, int16_t height, uint8_t frames = 1);

  // Returns a pointer to the sprite or nullptr if not created, user must cast to pointer type
  void *getPointer(void);

  // Returns true if sprite has been created
  bool created(void);

  // Delete the sprite to free up the RAM
  void deleteSprite(void);

  // Select the frame buffer for graphics write (for 2 colour ePaper and DMA toggle buffer)
  // Returns a pointer to the Sprite frame buffer
  void *frameBuffer(int8_t f);

  // Set or get the colour depth to 1, 4, 8 or 16 bits. Can be used to change depth an existing
  // sprite, but clears it to black, returns a new pointer if sprite is re-created.
  void *setColorDepth(int8_t b);
  int8_t getColorDepth(void);

  // Set the palette for a 4 bit depth sprite.  Only the first 16 colours in the map are used.
  void createPalette(uint16_t *palette = nullptr, uint8_t colors = 16);       // Palette in RAM
  void createPalette(const uint16_t *palette = nullptr, uint8_t colors = 16); // Palette in FLASH

  // Set a single palette index to the given color
  void setPaletteColor(uint8_t index, uint16_t color);

  // Get the color at the given palette index
  uint16_t getPaletteColor(uint8_t index);

  // Set foreground and background colours for 1 bit per pixel Sprite
  void setBitmapColor(uint16_t fg, uint16_t bg);

  // Draw a single pixel at x,y
  void drawPixel(int32_t x, int32_t y, uint32_t color) override;

  // Draw a single character in the GLCD or GFXFF font
  void drawChar(int32_t x, int32_t y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size) override;

  // Fill Sprite with a colour
  void fillSprite(uint32_t color);

  // Define a window to push 16 bit colour pixels into in a raster order
  // Colours are converted to the set Sprite colour bit depth
  void setWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1) override;
  // Push a color (aka singe pixel) to the sprite's set window area
  void pushColor(uint16_t color) override;
  // Push len colors (pixels) to the sprite's set window area
  void pushColor(uint16_t color, uint32_t len);
  // Push a pixel pre-formatted as a 1, 4, 8 or 16 bit colour (avoids conversion overhead)
  void writeColor(uint16_t color);

  // Set the scroll zone, top left corner at x,y with defined width and height
  // The colour (optional, black is default) is used to fill the gap after the scroll
  void setScrollRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color = TFT_BLACK);
  // Scroll the defined zone dx,dy pixels. Negative values left,up, positive right,down
  // dy is optional (default is 0, so no up/down scroll).
  // The sprite coordinate frame does not move because pixels are moved
  void scroll(int16_t dx, int16_t dy = 0);

  // Draw lines
  void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) override;
  void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color) override;
  void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) override;

  // Fill a rectangular area with a color (aka draw a filled rectangle)
  void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) override;

  // Set the coordinate rotation of the Sprite (for 1bpp Sprites only)
  // Note: this uses coordinate rotation and is primarily for ePaper which does not support
  // CGRAM rotation (like TFT drivers do) within the displays internal hardware
  void setRotation(uint8_t rotation);
  uint8_t getRotation(void);

  // Push a rotated copy of Sprite to TFT with optional transparent colour
  bool pushRotated(int16_t angle, uint32_t transp = 0x00FFFFFF);
  // Push a rotated copy of Sprite to another different Sprite with optional transparent colour
  bool pushRotated(TFT_eSprite *spr, int16_t angle, uint32_t transp = 0x00FFFFFF);

  // Get the TFT bounding box for a rotated copy of this Sprite
  bool getRotatedBounds(int16_t angle, int16_t *min_x, int16_t *min_y, int16_t *max_x, int16_t *max_y);
  // Get the destination Sprite bounding box for a rotated copy of this Sprite
  bool getRotatedBounds(TFT_eSprite *spr, int16_t angle, int16_t *min_x, int16_t *min_y,
                        int16_t *max_x, int16_t *max_y);
  // Bounding box support function
  void getRotatedBounds(int16_t angle, int16_t w, int16_t h, int16_t xp, int16_t yp,
                        int16_t *min_x, int16_t *min_y, int16_t *max_x, int16_t *max_y);

  // Read the colour of a pixel at x,y and return value in 565 format
  uint16_t readPixel(int32_t x0, int32_t y0) override;

  // return the numerical value of the pixel at x,y (used when scrolling)
  // 16bpp = colour, 8bpp = byte, 4bpp = colour index, 1bpp = 1 or 0
  uint16_t readPixelValue(int32_t x, int32_t y);

  // Write an image (colour bitmap) to the sprite.
  void pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data, uint8_t sbpp = 0);
  void pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, const uint16_t *data);

  // Push the sprite to the TFT screen, this fn calls pushImage() in the TFT class.
  // Optionally a "transparent" colour can be defined, pixels of that colour will not be rendered
  void pushSprite(int32_t x, int32_t y);
  void pushSprite(int32_t x, int32_t y, uint16_t transparent);

  // Push a windowed area of the sprite to the TFT at tx, ty
  bool pushSprite(int32_t tx, int32_t ty, int32_t sx, int32_t sy, int32_t sw, int32_t sh);

  // Push the sprite to another sprite at x,y. This fn calls pushImage() in the destination sprite (dspr) class.
  bool pushToSprite(TFT_eSprite *dspr, int32_t x, int32_t y);
  bool pushToSprite(TFT_eSprite *dspr, int32_t x, int32_t y, uint16_t transparent);

  // Draw a single character in the selected font
  int16_t drawChar(uint16_t uniCode, int32_t x, int32_t y, uint8_t font) override;
  int16_t drawChar(uint16_t uniCode, int32_t x, int32_t y) override;

  // Return the width and height of the sprite
  int16_t width(void) override;
  int16_t height(void) override;

  // Functions associated with anti-aliased fonts
  // Draw a single unicode character using the loaded font
  void drawGlyph(uint16_t code);
  // Print string to sprite using loaded font at cursor position
  void printToSprite(String string);
  // Print char array to sprite using loaded font at cursor position
  void printToSprite(char *cbuffer, uint16_t len);
  // Print indexed glyph to sprite using loaded font at x,y
  int16_t printToSprite(int16_t x, int16_t y, uint16_t index);

private:
  TFT_eSPI *_tft;

  // Reserve memory for the Sprite and return a pointer
  void *callocSprite(int16_t width, int16_t height, uint8_t frames = 1);

  // Override the non-inlined TFT_eSPI functions
  void begin_nin_write(void) override { ; }
  void end_nin_write(void) override { ; }

protected:
  int32_t _sinra{0}; // Sine of rotation angle in fixed point
  int32_t _cosra{0}; // Cosine of rotation angle in fixed point

  int32_t _xs{0}, _ys{0}, _xe{0}, _ye{0}, _xptr{0}, _yptr{0}; // for setWindow
  int32_t _sx{0}, _sy{0};                                     // x,y for scroll zone
  uint32_t _sw{0}, _sh{0};                                    // w,h for scroll zone
  uint32_t _scolor{0};                                        // gap fill colour for scroll zone

  int32_t _iwidth{0}, _iheight{0}; // Sprite memory image bit width and height (swapped during rotations)
  int32_t _dwidth{0}, _dheight{0}; // Real sprite width and height (for <8bpp Sprites)
  int32_t _bitwidth{0};            // Sprite image bit width for drawPixel (for <8bpp Sprites, not swapped)

  uint16_t *_img{nullptr};      // pointer to 16 bit sprite
  uint16_t *_colorMap{nullptr}; // color map pointer: 16 entries, used with 4 bit color map.
  uint8_t *_img8{nullptr};      // pointer to  1 and 8 bit sprite frame 1 or frame 2
  uint8_t *_img4{nullptr};      // pointer to  4 bit sprite (uses color map)
  uint8_t *_img8_1{nullptr};    // pointer to frame 1
  uint8_t *_img8_2{nullptr};    // pointer to frame 2

  uint8_t _bpp{16};     // bits per pixel (1, 4, 8 or 16)
  bool _created{false}; // A Sprite has been created and memory reserved
  bool _gFont{false};
};
