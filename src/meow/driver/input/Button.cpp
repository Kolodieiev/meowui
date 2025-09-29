#pragma GCC optimize("O3")

#include "Button.h"
#include <driver/touch_sens.h>
#include "esp32-hal-touch.h"
#include "hal/gpio_hal.h"
#include "meowui_setup/input_setup.h"

namespace meow
{
    Button::Button(uint8_t btn_id, bool is_touch) : _btn_id{btn_id},
                                                    _is_touch{is_touch}
    {
        enable();
    }

    void Button::lock(unsigned long lock_duration)
    {
        _is_locked = true;
        _is_holded = false;
        _is_released = false;
        _is_pressed = false;
        _is_touched = false;
        _lock_duration = lock_duration;
        _lock_time = millis();
    }

    void Button::__update()
    {
        if (!_is_enabled)
            return;

        if (_is_locked)
        {
            if (millis() - _lock_time < _lock_duration)
                return;
            _is_locked = false;
        }

        if (_is_touch)
        {
#if defined(CONFIG_IDF_TARGET_ESP32S3)
            _is_holded = touchRead(_btn_id) > BTN_TOUCH_TRESHOLD;
#else
            _is_holded = touchRead(_btn_id) < BTN_TOUCH_TRESHOLD;
#endif
        }
        else
        {
            // TODO тест на esp32
#if defined(CONFIG_IDF_TARGET_ESP32S3)
            _is_holded = (_btn_id < 32) ? !((GPIO.in >> _btn_id) & 1) : !((GPIO.in1.val >> (_btn_id - 32)) & 1);
#else
            _is_holded = !gpio_get_level((gpio_num_t)_btn_id);
#endif
        }

        updateState();
    }

    void Button::__extUpdate(bool is_holded)
    {
        if (!_is_enabled)
            return;

        if (_is_locked)
        {
            if (millis() - _lock_time < _lock_duration)
                return;
            _is_locked = false;
        }

        _is_holded = is_holded;

        updateState();
    }

    void Button::updateState()
    {
        if (!_is_holded)
        {
            if (_is_touched)
            {
                _is_touched = false;
                _is_released = true;
            }

            return;
        }

        if (_is_touched)
        {
            if (!_is_pressed)
                if (millis() - _hold_duration > PRESS_DURATION)
                {
                    _is_touched = false;
                    _is_pressed = true;
                }
        }
        else
        {
            _is_touched = true;
            _is_released = false;
            _hold_duration = millis();
        }
    }

    void Button::reset()
    {
        _is_holded = false;
        _is_pressed = false;
        _is_released = false;
    }

    void Button::enable()
    {
        _is_enabled = true;

#ifndef EXT_INPUT
        if (!_is_touch)
        {
            pinMode(_btn_id, INPUT_PULLUP);
        }
        else if (!touchRead(_btn_id))
        {
            log_e("Помилка ініціалізації сенсорної кнопки");
            esp_restart();
        }
#endif
    }

    void Button::disable()
    {
        _is_enabled = false;
        reset();

        gpio_reset_pin((gpio_num_t)_btn_id);
        pinMode(_btn_id, INPUT);
    }
}