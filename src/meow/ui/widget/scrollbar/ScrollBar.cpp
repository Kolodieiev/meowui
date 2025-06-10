#pragma GCC optimize("O3")

#include "ScrollBar.h"

namespace meow
{
    ScrollBar::ScrollBar(uint16_t widget_ID) : IWidget(widget_ID, TYPE_ID_SCROLLBAR) {}

    ScrollBar *ScrollBar::clone(uint16_t id) const
    {
        try
        {
            ScrollBar *cln = new ScrollBar(id);
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

            cln->_max_value = _max_value;
            cln->_cur_value = _cur_value;
            cln->_slider_color = _slider_color;
            cln->_orientation = _orientation;
            cln->_slider_last_x_pos = _slider_last_x_pos;
            cln->_slider_last_y_pos = _slider_last_y_pos;
            cln->_slider_width = _slider_width;
            cln->_slider_height = _slider_height;
            cln->_slider_step_size = _slider_step_size;
            cln->_smart_scroll_enabled = _smart_scroll_enabled;
            cln->_smart_value = _smart_value;
            cln->_steps_to_scroll = _steps_to_scroll;
            cln->_steps_counter = _steps_counter;
            cln->_scroll_direction = _scroll_direction;

            return cln;
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
            _is_changed = true;
            ++_cur_value;

            if (_smart_scroll_enabled)
            {
                if (_scroll_direction == 0)
                {
                    if (_steps_counter < _steps_to_scroll - 1)
                    {
                        ++_steps_counter;
                    }
                    else
                    {
                        _steps_counter = 0;
                        ++_smart_value;
                    }
                }
                else if (_steps_counter > 0)
                {
                    --_steps_counter;
                }
                else
                {
                    _scroll_direction = 0;
                    _steps_counter = 1;
                }
            }

            return true;
        }
        return false;
    }

    bool ScrollBar::scrollUp()
    {
        if (_cur_value > 0)
        {
            _is_changed = true;
            --_cur_value;

            if (_smart_scroll_enabled)
            {
                if (_scroll_direction == 1)
                {
                    if (_steps_counter < _steps_to_scroll - 1)
                    {
                        ++_steps_counter;
                    }
                    else
                    {
                        _steps_counter = 0;
                        --_smart_value;
                    }
                }
                else if (_steps_counter > 0)
                {
                    --_steps_counter;
                }
                else
                {
                    _scroll_direction = 1;
                    _steps_counter = 1;
                }
            }

            return true;
        }
        return false;
    }
}