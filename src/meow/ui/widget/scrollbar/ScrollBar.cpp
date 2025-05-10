#pragma GCC optimize("O3")

#include "ScrollBar.h"

namespace meow
{
    ScrollBar::ScrollBar(uint16_t widget_ID) : IWidget(widget_ID, TYPE_ID_SCROLLBAR) {}

    ScrollBar *ScrollBar::clone(uint16_t id) const
    {
        try
        {
            ScrollBar *clone = new ScrollBar(id);
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

            clone->_max_value = _max_value;
            clone->_cur_value = _cur_value;
            clone->_slider_color = _slider_color;
            clone->_orientation = _orientation;
            clone->_slider_last_x_pos = _slider_last_x_pos;
            clone->_slider_last_y_pos = _slider_last_y_pos;
            clone->_slider_width = _slider_width;
            clone->_slider_height = _slider_height;
            clone->_slider_step_size = _slider_step_size;
            clone->_smart_scroll_enabled = _smart_scroll_enabled;
            clone->_smart_value = _smart_value;
            clone->_steps_to_scroll = _steps_to_scroll;
            clone->_steps_counter = _steps_counter;
            clone->_scroll_direction = _scroll_direction;

            return clone;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }

    void ScrollBar::setValue(uint16_t value)
    {
        _cur_value = value > _max_value ? _max_value : value;
        _is_changed = true;
    }

    void ScrollBar::onDraw()
    {
        if (!_is_changed)
            return;

        _is_changed = false;

        if (_visibility == INVISIBLE)
        {
            hide();
            return;
        }

        uint16_t x_offset{0};
        uint16_t y_offset{0};

        if (_parent)
        {
            x_offset = _parent->getXPos();
            y_offset = _parent->getYPos();
        }

        uint16_t slider_x_pos = _x_pos + x_offset;
        uint16_t slider_y_pos = _y_pos + y_offset;

        uint16_t step_value = _smart_scroll_enabled ? _smart_value : _cur_value;

        if (_orientation == VERTICAL)
            slider_y_pos = _y_pos + y_offset + _slider_step_size * step_value;
        else
            slider_x_pos = _x_pos + x_offset + _slider_step_size * step_value;

        // fill body
        _display.fillRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _back_color);

        // clear old
        if (_slider_last_x_pos < _x_pos + x_offset)
            _slider_last_x_pos = _x_pos + x_offset;
        if (_slider_last_y_pos < _y_pos + y_offset)
            _slider_last_y_pos = _y_pos + y_offset;

        uint16_t old_slider_w = _slider_width;
        if (_slider_last_x_pos + _slider_width > _x_pos + x_offset + _width)
            old_slider_w = _x_pos + x_offset + _width - _slider_last_x_pos;

        uint16_t old_slider_h = _slider_height;
        if (_slider_last_y_pos + _slider_height > _y_pos + y_offset + _height)
            old_slider_h = _y_pos + y_offset + _height - _slider_last_y_pos;

        _display.fillRect(_slider_last_x_pos, _slider_last_y_pos, old_slider_w, old_slider_h, _back_color);

        _slider_last_x_pos = slider_x_pos;
        _slider_last_y_pos = slider_y_pos;

        // draw new
        _display.fillRect(slider_x_pos, slider_y_pos, _slider_width, _slider_height, _slider_color);
    }

    void ScrollBar::setMax(uint16_t max_value)
    {
        _max_value = max_value > 0 ? max_value : 1;

        if (_cur_value > _max_value)
            _cur_value = _max_value;

        if (_orientation == VERTICAL)
        {
            _slider_width = _width;

            float temp_slider_height = (float)_height / _max_value;

            if (temp_slider_height >= 1.0f)
            {
                _smart_scroll_enabled = false;
                _slider_step_size = floor(temp_slider_height);
                _slider_height = _slider_step_size + _height - _slider_step_size * _max_value;
            }
            else
            {
                _steps_to_scroll = ceil((float)_max_value / _height); // кількість прокруток до зміни позиції слайдера.
                uint16_t positions_number = floor((double)_max_value / _steps_to_scroll);

                _slider_height = 1 + _height - positions_number;
                _slider_step_size = 1;

                _smart_scroll_enabled = true;
            }
        }
        else
        {
            _slider_height = _height;

            float temp_slider_width = (float)_width / _max_value;

            if (temp_slider_width >= 1.0f)
            {
                _smart_scroll_enabled = false;
                _slider_step_size = floor(temp_slider_width);
                _slider_width = _slider_step_size + _width - _slider_step_size * _max_value;
            }
            else
            {
                _steps_to_scroll = ceil((float)_max_value / _width);
                uint16_t positions_number = floor((double)_max_value / _steps_to_scroll);

                _slider_width = 1 + _width - positions_number;
                _slider_step_size = 1;

                _smart_scroll_enabled = true;
            }
        }

        _is_changed = true;
    }

    bool ScrollBar::scrollDown()
    {
        if (_cur_value < _max_value - 1)
        {
            _cur_value++;

            if (!_smart_scroll_enabled)
                _is_changed = true;
            else if (_scroll_direction == 0)
            {
                if (_steps_counter < _steps_to_scroll - 1)
                    _steps_counter++;
                else
                {
                    _is_changed = true;
                    _steps_counter = 0;
                    _smart_value++;
                }
            }
            else if (_steps_counter > 0)
                _steps_counter--;
            else
            {
                _scroll_direction = 0;
                _steps_counter = 1;
            }

            _is_changed = true;
            return true;
        }
        return false;
    }

    bool ScrollBar::scrollUp()
    {
        if (_cur_value > 0)
        {
            _cur_value--;

            if (!_smart_scroll_enabled)
                _is_changed = true;
            else if (_scroll_direction == 1)
            {
                if (_steps_counter < _steps_to_scroll - 1)
                    _steps_counter++;
                else
                {
                    _is_changed = true;
                    _steps_counter = 0;
                    _smart_value--;
                }
            }
            else if (_steps_counter > 0)
                _steps_counter--;
            else
            {
                _scroll_direction = 1;
                _steps_counter = 1;
            }

            _is_changed = true;
            return true;
        }
        return false;
    }
}