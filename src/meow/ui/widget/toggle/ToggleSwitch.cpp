#include "ToggleSwitch.h"

namespace meow
{

    void ToggleSwitch::onDraw()
    {
        if (_is_changed)
        {
            _is_changed = false;

            if (_visibility == INVISIBLE)
            {
                hide();
                return;
            }

            if (_is_on)
                _back_color = _on_color;
            else
                _back_color = _off_color;

            clear();

            //
            uint16_t lever_w;
            uint16_t lever_h;

            if (_width > _height)
            {
                lever_w = _height - 2;
                lever_h = lever_w - 4;
            }
            else
            {
                lever_h = _width - 2;
                lever_w = lever_h - 4;
            }

            uint16_t lever_x;
            uint16_t lever_y;

            if (_orientation == ORIENTATION_HORIZONTAL)
            {
                lever_x = _is_on ? _x_pos + _width - lever_w - 3 : _x_pos + 3;
                lever_y = _y_pos + 3;
            }
            else
            {
                lever_x = _x_pos + 3;
                lever_y = _is_on ? _y_pos + 3 : _y_pos + _height - lever_h - 3;
            }

            uint16_t x_offset{0};
            uint16_t y_offset{0};

            if (_parent)
            {
                x_offset = _parent->getXPos();
                y_offset = _parent->getYPos();
            }

            if (_corner_radius == 0)
                _display.fillRect(lever_x + x_offset, lever_y + y_offset, lever_w, lever_h, _lever_color);
            else
                _display.fillRoundRect(lever_x + x_offset, lever_y + y_offset, lever_w, lever_h, _corner_radius, _lever_color);
        }
    }

    ToggleSwitch *ToggleSwitch::clone(uint16_t id) const
    {
        try
        {
            ToggleSwitch *clone = new ToggleSwitch(*this);
            clone->_id = id;
            return clone;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }
}
