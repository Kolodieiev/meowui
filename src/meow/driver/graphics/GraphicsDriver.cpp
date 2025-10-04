#pragma GCC optimize("O3")

#include "GraphicsDriver.h"
#include "meow/util/img/BmpUtil.h"
#include "esp32-hal-ledc.h"

namespace meow
{
#ifdef DOUBLE_BUFFERRING

#ifdef ENABLE_SCREENSHOTER
    void GraphicsDriver::takeScreenshot()
    {
        _take_screenshot = true;
    }
#endif // ENABLE_SCREENSHOTER

    GraphicsDriver::GraphicsDriver() : _sync_mutex{xSemaphoreCreateMutex()}
    {
        xTaskCreatePinnedToCore(displayRendererTask, "dRend", 5 * 512, this, 11, nullptr, 0);
    }

    void GraphicsDriver::init()
    {
#ifdef GRAPHICS_ENABLED
        _tft.setRotation(DISPLAY_ROTATION);

        _tft.init();

        if (!psramInit())
        {
            log_e("Помилка. PSRAM відсутня або не працює");
            esp_restart();
        }

#if defined(COLOR_16BIT)
        _flick_buf.setColorDepth(16);
        _renderer_buf.setColorDepth(16);
#elif defined(COLOR_8BIT)
        _flick_buf.setColorDepth(8);
        _renderer_buf.setColorDepth(8);
#elif defined(COLOR_1BIT)
        _flick_buf.setColorDepth(1);
        _renderer_buf.setColorDepth(1);
#endif
        _flick_buf.setAttribute(PSRAM_ENABLE, true);
        _renderer_buf.setAttribute(PSRAM_ENABLE, true);
        //
        _flick_buf.createSprite(_tft.width(), _tft.height(), 1);
        _renderer_buf.createSprite(_tft.width(), _tft.height(), 1);

        if (!_flick_buf.getPointer() || !_renderer_buf.getPointer())
        {
            log_e("Помилка створення буферів малювання");
            esp_restart();
        }

#else
        log_e("Розкоментуй відповідний прапор в налаштуваннях драйвера графіки");
        esp_restart();
#endif // GRAPHICS_ENABLED
    }

    void GraphicsDriver::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data)
    {
        _is_buff_changed = true;
        _flick_buf.setSwapBytes(true);
        _flick_buf.pushImage(x, y, w, h, data);
        _flick_buf.setSwapBytes(false);
    }

    void GraphicsDriver::pushRotated(TFT_eSprite &sprite, uint16_t x_pivot, uint16_t y_pivot, int16_t angle, uint32_t transparent_color)
    {
        _is_buff_changed = true;
        _flick_buf.setPivot(x_pivot, y_pivot);
        sprite.pushRotated(&_flick_buf, angle, transparent_color);
        _flick_buf.setPivot(0, 0);
    }

    void GraphicsDriver::__pushBuffer()
    {
        if (_is_buff_changed)
        {
#ifdef SHOW_FPS
            if (millis() - _frame_timer < 1000)
            {
                ++_temp_frame_counter;
            }
            else
            {
                _frame_counter = _temp_frame_counter + 1;
                _temp_frame_counter = 0;
                _frame_timer = millis();
            }

            uint16_t fps_x_pos = _tft.width() / 2 - 20;
            _flick_buf.fillRect(fps_x_pos, 0, 20, 12, TFT_BLACK);
            _flick_buf.setTextColor(TFT_RED);
            _flick_buf.setTextFont(4);
            _flick_buf.setTextSize(1);
            _flick_buf.drawString(String(_frame_counter), fps_x_pos, 0);

#endif // SHOW_FPS
            _is_buff_changed = false;

            xSemaphoreTake(_sync_mutex, portMAX_DELAY);
            _has_frame = true;
#if defined(COLOR_16BIT)
            _flick_buf.fastCopy16(_renderer_buf);
#else
            _flick_buf.fastCopy8(_renderer_buf);
#endif
            xSemaphoreGive(_sync_mutex);
        }
    }

    void GraphicsDriver::displayRendererTask(void *params)
    {
        GraphicsDriver &self = *static_cast<GraphicsDriver *>(params);

        while (1)
        {
            if (self._has_frame)
            {
                xSemaphoreTake(self._sync_mutex, portMAX_DELAY);

                self._has_frame = false;
                self._renderer_buf.pushSprite(0, 0);

#ifdef ENABLE_SCREENSHOTER
                if (self._take_screenshot)
                {
                    self._take_screenshot = false;

                    BmpHeader header;
                    header.width = self._renderer_buf.width();
                    header.height = self._renderer_buf.height();

                    String path_to_bmp = "/screenshot_";
                    path_to_bmp += millis();
                    path_to_bmp += ".bmp";

                    bool res = BmpUtil::saveBmp(header, static_cast<uint16_t *>(self._renderer_buf.getPointer()), path_to_bmp.c_str());

                    if (res)
                        log_i("Скріншот успішно збережено");
                    else
                        log_e("Помилка створення скріншоту");
                }
#endif // ENABLE_SCREENSHOTER
                xSemaphoreGive(self._sync_mutex);
            }
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }

#else  // not DOUBLE_BUFFERRING
    GraphicsDriver::GraphicsDriver()
    {
    }

    void GraphicsDriver::init()
    {
        _tft.setRotation(DISPLAY_ROTATION);
        _tft.init();
    }

    void GraphicsDriver::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data)
    {
        _tft.setSwapBytes(true);
        _tft.pushImage(x, y, w, h, data);
        _tft.setSwapBytes(false);
    }

    void GraphicsDriver::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data, uint16_t transparent)
    {
        _tft.setSwapBytes(true);
        _tft.pushImage(x, y, w, h, data, transparent);
        _tft.setSwapBytes(false);
    }

    void GraphicsDriver::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data)
    {
        _tft.setSwapBytes(true);
        _tft.pushImage(x, y, w, h, data);
        _tft.setSwapBytes(false);
    }

    void GraphicsDriver::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data, uint16_t transparent)
    {
        _tft.setSwapBytes(true);
        _tft.pushImage(x, y, w, h, data, transparent);
        _tft.setSwapBytes(false);
    }
#endif // DOUBLE_BUFFERRING

#ifdef BACKLIGHT_PIN
    void meow::GraphicsDriver::enableBackLight()
    {
        pinMode(BACKLIGHT_PIN, OUTPUT);
        digitalWrite(BACKLIGHT_PIN, HIGH);
    }

    void meow::GraphicsDriver::disableBackLight()
    {
        digitalWrite(BACKLIGHT_PIN, LOW);
    }

    void meow::GraphicsDriver::setBrightness(uint8_t value)
    {
        _cur_brightness = value;
        ledcAttach(BACKLIGHT_PIN, PWM_FREQ, PWM_RESOLUTION);
        ledcWrite(BACKLIGHT_PIN, value);
    }
#endif // BACKLIGHT_PIN

    GraphicsDriver _display;
}
