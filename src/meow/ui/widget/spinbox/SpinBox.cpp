#pragma GCC optimize("O3")

#include "SpinBox.h"

namespace meow
{

    SpinBox::SpinBox(uint16_t widget_ID, GraphicsDriver &display) : Label(widget_ID, display, CLASS_ID_SPINBOX)
    {
        setAlign(IWidget::ALIGN_CENTER);
        setGravity(IWidget::GRAVITY_CENTER);
    }

    SpinBox *SpinBox::clone(uint16_t id) const
    {
        try
        {
            SpinBox *clone = new SpinBox(id, _display);
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

            clone->_is_multiline = _is_multiline;
            clone->setText(_text);
            clone->_text_size = _text_size;
            clone->_text_color = _text_color;
            clone->_font_ID = _font_ID;
            clone->_text_offset = _text_offset;
            clone->_text_gravity = _text_gravity;
            clone->_text_alignment = _text_alignment;
            clone->_is_ticker = _is_ticker;
            clone->_temp_is_ticker = _temp_is_ticker;
            clone->_is_ticker_in_focus = _is_ticker_in_focus;
            clone->_temp_is_ticker_in_focus = _temp_is_ticker_in_focus;
            clone->_temp_width = _temp_width;

            if (_back_img)
            {
                clone->_back_img = _back_img->clone(_back_img->getID());
            }

            clone->_min = _min;
            clone->_max = _max;
            clone->_value = _value;
            clone->_spin_type;

            return clone;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }

    void SpinBox::setMin(float min)
    {
        _min = min;

        if (_value < _min)
        {
            _value = _min;
        }

        setSpinValToDraw();
        _is_changed = true;
    }

    void SpinBox::setMax(float max)
    {
        _max = max;
        if (_value > _max)
        {
            _value = _max;
        }

        setSpinValToDraw();
        _is_changed = true;
    }

    void SpinBox::setValue(float value)
    {
        if (value < _min)
            _value = _min;
        else if (value > _max)
            _value = _max;
        else
            _value = value;

        setSpinValToDraw();
    }

    void SpinBox::setType(SpinType spin_type)
    {
        _spin_type = spin_type;
        _is_changed = true;
        setSpinValToDraw();
    }

    void SpinBox::setSpinValToDraw()
    {
        if (_spin_type == TYPE_INT)
        {
            uint64_t temp = _value;
            setText(String(temp));
        }
        else
            setText(String(_value));
    }

    void SpinBox::up()
    {
        if (_value < _max)
            _value++;
        else
            _value = _min;

        setSpinValToDraw();
    }

    void SpinBox::down()
    {
        if (_value > _min)
            _value--;
        else
            _value = _max;

        setSpinValToDraw();
    }

}