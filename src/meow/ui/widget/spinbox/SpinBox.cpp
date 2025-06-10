#pragma GCC optimize("O3")

#include "SpinBox.h"

namespace meow
{

    SpinBox::SpinBox(uint16_t widget_ID) : Label(widget_ID, TYPE_ID_SPINBOX)
    {
        setAlign(IWidget::ALIGN_CENTER);
        setGravity(IWidget::GRAVITY_CENTER);
    }

    SpinBox *SpinBox::clone(uint16_t id) const
    {
        try
        {
            SpinBox *cln = new SpinBox(id);
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

            cln->setText(_text);
            cln->_text_size = _text_size;
            cln->_text_color = _text_color;
            cln->_font_ID = _font_ID;
            cln->_text_offset = _text_offset;
            cln->_temp_width = _temp_width;

            if (_back_img)
            {
                cln->_back_img = _back_img->clone(_back_img->getID());
            }

            cln->_min = _min;
            cln->_max = _max;
            cln->_value = _value;
            cln->_spin_type = _spin_type;

            return cln;
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