#include "ToggleSwitch.h"

namespace meow
{
    void ToggleSwitch::setOrientation(Orientation orientation)
    {
        _orientation = orientation;
        _is_changed = true;
    }

    void ToggleSwitch::setOffColor(uint16_t color)
    {
        _off_color = color;
        _is_changed = true;
    }

    void ToggleSwitch::setLeverColor(uint16_t color)
    {
        _lever_color = color;
        _is_changed = true;
    }

    void ToggleSwitch::setOn(bool state)
    {
        _is_on = state;
        _is_changed = true;
    }

    void ToggleSwitch::toggle()
    {
        _is_on = !_is_on;
        _is_changed = true;
    }

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

            if (_orientation == HORIZONTAL)
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
            ToggleSwitch *cln = new ToggleSwitch(id);
            cln->_has_border = _has_border;
            cln->_x_pos = _x_pos;
            cln->_y_pos = _y_pos;
            cln->_width = _width;
            cln->_height = _height;
            cln->_back_color = _back_color;
            cln->_border_color = _border_color;
            cln->_corner_radius = _corner_radius;
            cln->_is_transparent = _is_transparent;
            cln->_visibility = _visibility;
            cln->_has_focus = _has_focus;
            cln->_old_border_state = _old_border_state;
            cln->_need_clear_border = _need_clear_border;
            cln->_need_change_border = _need_change_border;
            cln->_need_change_back = _need_change_back;
            cln->_focus_border_color = _focus_border_color;
            cln->_old_border_color = _old_border_color;
            cln->_focus_back_color = _focus_back_color;
            cln->_old_back_color = _old_back_color;
            cln->_parent = _parent;

            cln->_orientation = _orientation;
            cln->_is_on = _is_on;
            cln->_lever_color = _lever_color;
            cln->_on_color = _on_color;
            cln->_off_color = _off_color;

            return cln;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }
}
