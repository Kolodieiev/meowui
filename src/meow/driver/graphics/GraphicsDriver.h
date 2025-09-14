#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include "./TFT_eSPI/TFT_eSPI.h"
#include "meowui_setup/graphics_setup.h"

namespace meow
{
        class GraphicsDriver
        {
        public:
                GraphicsDriver();

                void init();
                void setRotation(uint8_t rotation) { _tft.setRotation(rotation); }
                //
                uint16_t width() { return _tft.width(); }
                uint16_t height() { return _tft.height(); }

                // ---------------------------------------------------------------------
                void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
                {
#ifdef DOUBLE_BUFFERRING
                        _is_buffer_changed = true;
                        _flick_buf.fillRect(x, y, w, h, color);
#else
                        _tft.fillRect(x, y, w, h, color);
#endif
                }

                void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint32_t color)
                {
#ifdef DOUBLE_BUFFERRING
                        _is_buffer_changed = true;
                        _flick_buf.fillRoundRect(x, y, w, h, radius, color);
#else
                        _tft.fillRoundRect(x, y, w, h, radius, color);
#endif
                }

                void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
                {
#ifdef DOUBLE_BUFFERRING
                        _is_buffer_changed = true;
                        _flick_buf.drawRect(x, y, w, h, color);
#else
                        _tft.drawRect(x, y, w, h, color);
#endif
                }

                void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint32_t color)
                {
#ifdef DOUBLE_BUFFERRING
                        _is_buffer_changed = true;
                        _flick_buf.drawRoundRect(x, y, w, h, radius, color);
#else
                        _tft.drawRoundRect(x, y, w, h, radius, color);
#endif
                }

                //
                void fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
                {
#ifdef DOUBLE_BUFFERRING
                        _is_buffer_changed = true;
                        _flick_buf.fillTriangle(x0, y0, x1, y1, x2, y2, color);
#else
                        _tft.fillTriangle(x0, y0, x1, y1, x2, y2, color);
#endif
                }

                //
                void setTextFont(uint8_t font_ID)
                {
#ifdef DOUBLE_BUFFERRING
                        _is_buffer_changed = true;
                        _flick_buf.setTextFont(font_ID);
#else
                        _tft.setTextFont(font_ID);
#endif
                }

                void setTextSize(uint8_t size)
                {
#ifdef DOUBLE_BUFFERRING
                        _is_buffer_changed = true;
                        _flick_buf.setTextSize(size);
#else
                        _tft.setTextSize(size);
#endif
                }

                void setTextColor(uint16_t color)
                {
#ifdef DOUBLE_BUFFERRING
                        _is_buffer_changed = true;
                        _flick_buf.setTextColor(color);
#else
                        _tft.setTextColor(color);
#endif
                }

                int16_t drawString(const char *string, int32_t x, int32_t y)
                {
#ifdef DOUBLE_BUFFERRING
                        _is_buffer_changed = true;
                        return _flick_buf.drawString(string, x, y);
#else
                        return _tft.drawString(string, x, y);
#endif
                }

                int16_t drawString(const String &string, int32_t x, int32_t y)
                {
#ifdef DOUBLE_BUFFERRING
                        _is_buffer_changed = true;
                        return _flick_buf.drawString(string, x, y);
#else
                        return _tft.drawString(string, x, y);
#endif
                }

                TFT_eSPI *getTFT() { return &_tft; }

                void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data);

#ifdef DOUBLE_BUFFERRING
                void pushSprite(TFT_eSprite &sprite, int32_t x, int32_t y)
                {
                        _is_buffer_changed = true;
                        sprite.pushToSprite(&_flick_buf, x, y);
                }

                void pushSprite(TFT_eSprite &sprite, int32_t x, int32_t y, uint16_t transparent_color)
                {
                        _is_buffer_changed = true;
                        sprite.pushToSprite(&_flick_buf, x, y, transparent_color);
                }

                void pushRotated(TFT_eSprite &sprite, uint16_t x_pivot, uint16_t y_pivot, int16_t angle, uint32_t transparent_color);
#ifdef ENABLE_SCREENSHOTER
                void takeScreenshot();
#endif
                // Не викликай цей метод самостійно
                void _pushBuffer();
#else
                //
                //  RAM
                //
                void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data);
                void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data, uint16_t transparent);
                //
                // FLASH
                //
                void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data, uint16_t transparent);
#endif

#ifdef BACKLIGHT_PIN
                /**
                 * @brief Вмикає підсвітку дисплея зі 100% яскравістю.
                 *
                 */
                void enableBackLight();

                /**
                 * @brief Вимикає підсвітку дисплея
                 *
                 */
                void disableBackLight();

                /**
                 * @brief Встановлює яскравість підсвітки дисплея.
                 * Де 0 - підсвітка вимкнена, а 255 - рівень яскравості максимальний.
                 *
                 * @param value
                 */
                void setBrightness(uint8_t value);

                /**
                 * @brief Повертає значення поточної яскравості підсвітки дисплея.
                 *
                 * @return uint8_t
                 */
                uint8_t getBrightness() const { return _cur_brightness; }

        private:
                uint8_t _cur_brightness = 125;
#endif

        private:
                TFT_eSPI _tft = TFT_eSPI();

#ifdef DOUBLE_BUFFERRING
#ifdef SHOW_FPS
                uint16_t _frame_counter{0};
                uint16_t _temp_frame_counter{0};
                uint64_t _frame_timer{0};
#endif
                bool _is_buffer_changed = false;
                TFT_eSprite _flick_buf = TFT_eSprite(&_tft);
                TFT_eSprite _renderer_buf = TFT_eSprite(&_tft);

                static TFT_eSprite *_rend_buf_ptr;

                volatile static xSemaphoreHandle _sync_mutex;
                volatile static bool _has_frame;
                volatile static bool _take_screenshot;

                static void displayRendererTask(void *params);
#endif
        };

        extern GraphicsDriver _display;
}
