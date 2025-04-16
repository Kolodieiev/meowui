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
            ToggleSwitch *clone = new ToggleSwitch(id, _display);
            clone->_has_border = _has_border;
            clone->_x_pos = _x_pos;
            clone->_y_pos = _y_pos;
            clone->_width = _width;
            clone->_height = _height;
            clone->_back_color = _back_color;
            clone->_border_color = _border_color;
            clone->_corner_radius = _corner_radius;
            clone->_is_transparent = _is_transparent;
            clone->_visibility = _visibility;
            clone->_has_focus = _has_focus;
            clone->_old_border_state = _old_border_state;
            clone->_need_clear_border = _need_clear_border;
            clone->_need_change_border = _need_change_border;
            clone->_need_change_back = _need_change_back;
            clone->_focus_border_color = _focus_border_color;
            clone->_old_border_color = _old_border_color;
            clone->_focus_back_color = _focus_back_color;
            clone->_old_back_color = _old_back_color;
            clone->_parent = _parent;

            clone->_orientation = _orientation;
            clone->_is_on = _is_on;
            clone->_lever_color = _lever_color;
            clone->_on_color = _on_color;
            clone->_off_color = _off_color;

            return clone;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }
}
