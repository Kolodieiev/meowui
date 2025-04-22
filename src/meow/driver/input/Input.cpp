#pragma GCC optimize("O3")

#include "Input.h"

#include "soc/io_mux_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_struct.h"
#include "soc/gpio_periph.h"

#ifdef GT911_DRIVER
#include "./touchscreen_driver/GT911.h"
#endif

const char STR_UNKNOWN_PIN[] = "Незареєстрована віртуальна кнопка";

namespace meow
{
        Input::Input()
        {
#ifdef TOUCHSCREEN_SUPPORT
#ifdef GT911_DRIVER
                GT911 *gt = new GT911();
                gt->setRotation(TOUCH_ROTATION);
                gt->begin(TOUCH_SDA_PIN, TOUCH_SCL_PIN, TOUCH_INT_PIN, TOUCH_RST_PIN, TOUCH_WIDTH, TOUCH_HEIGHT);
                _touchscreen = gt;
#endif // GT911_DRIVER

#endif // TOUCHSCREEN_SUPPORT
        }

        void Input::_update()
        {
#ifdef TOUCHSCREEN_SUPPORT
                _touchscreen->update();
#endif

                for (auto &&btn : _buttons)
                        btn.second->_update();
        }

        void Input::reset()
        {
#ifdef TOUCHSCREEN_SUPPORT
                _touchscreen->reset();
#endif

                for (auto &&btn : _buttons)
                        btn.second->reset();
        }

        void Input::_printPinMode(KeyID key_id)
        {
                if ((gpio_num_t)key_id >= GPIO_NUM_MAX)
                {
                        log_i("Invalid pin number: %d", key_id);
                        return;
                }

                uint32_t io_mux_reg = GPIO_PIN_MUX_REG[key_id]; // Отримати адресу IOMUX регістра

                if (REG_GET_BIT(io_mux_reg, FUN_PU))
                        log_i("Pin %d: Pull-up enabled", key_id);
                else if (REG_GET_BIT(io_mux_reg, FUN_PD))
                        log_i("Pin %d: Pull-down enabled", key_id);
                else
                        log_i("Pin %d: is floating", key_id);
        }

        void Input::enablePin(KeyID key_id)
        {
                try
                {
                        _buttons.at(key_id)->enable();
                }
                catch (const std::out_of_range &ignored)
                {
                        log_e("%s", STR_UNKNOWN_PIN);
                }
        }

        void Input::disablePin(KeyID key_id)
        {
                try
                {
                        _buttons.at(key_id)->disable();
                }
                catch (const std::out_of_range &ignored)
                {
                        log_e("%s", STR_UNKNOWN_PIN);
                }
        }

        bool Input::isHolded(KeyID key_id) const
        {
                try
                {
                        return _buttons.at(key_id)->isHolded();
                }
                catch (const std::out_of_range &ignored)
                {
                        log_e("%s", STR_UNKNOWN_PIN);
                        return false;
                }
        }

        bool Input::isPressed(KeyID key_id) const
        {
                try
                {
                        return _buttons.at(key_id)->isPressed();
                }
                catch (const std::out_of_range &ignored)
                {
                        log_e("%s", STR_UNKNOWN_PIN);
                        return false;
                }
        }

        bool Input::isReleased(KeyID key_id) const
        {
                try
                {
                        return _buttons.at(key_id)->isReleased();
                }
                catch (const std::out_of_range &ignored)
                {
                        log_e("%s", STR_UNKNOWN_PIN);
                        return false;
                }
        }

        void Input::lock(KeyID key_id, unsigned long lock_duration)
        {
                try
                {
                        _buttons.at(key_id)->lock(lock_duration);
                }
                catch (const std::out_of_range &ignored)
                {
                        log_e("%s", STR_UNKNOWN_PIN);
                }
        }
}