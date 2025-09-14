#include "ComboItem.h"

namespace meow
{
    ComboItem::ComboItem(uint16_t widget_ID) : MenuItem(widget_ID, TYPE_ID_COMBO_ITEM)
    {
    }

    ComboItem::~ComboItem()
    {
    }

    ComboItem *ComboItem::clone(uint16_t id) const
    {
        try
        {
            ComboItem *cln = new ComboItem(id);
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

            if (_img)
                cln->setImg(_img->clone(_img->getID()));

            cln->setLbl(_label->clone(_label->getID()));

            cln->_range = _range;
            cln->_curr_pos = _curr_pos;

            return cln;
        }
        catch (const std::bad_alloc &e)
        {

            log_e("%s", e.what());
            esp_restart();
        }
    }

    void ComboItem::setRange(const std::vector<String> &range)
    {
        if (range.empty())
        {
            log_e("Вектор не може бути порожім");
            esp_restart();
        }

        _range = range;
        _curr_pos = 0;
        _label->setText(range[0]);
    }

    void ComboItem::selectPos(uint16_t pos)
    {
        if (_range.empty())
            return;

        if (pos >= _range.size())
            pos = _range.size() - 1;

        _curr_pos = pos;
        _label->setText(_range[_curr_pos]);
    }

    void ComboItem::scrollUp()
    {
        if (_range.empty())
            return;

        if (_curr_pos == 0)
            _curr_pos = _range.size() - 1;
        else
            --_curr_pos;

        _label->setText(_range[_curr_pos]);
    }

    void ComboItem::scrollDown()
    {
        if (_range.empty())
            return;

        if (_curr_pos < _range.size() - 1)
            ++_curr_pos;
        else
            _curr_pos = 0;

        _label->setText(_range[_curr_pos]);
    }
}