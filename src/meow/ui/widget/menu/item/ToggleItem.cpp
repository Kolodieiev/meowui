#pragma GCC optimize("O3")

#include "ToggleItem.h"
namespace meow
{
    ToggleItem::ToggleItem(uint16_t widget_ID) : MenuItem(widget_ID, TYPE_ID_TOGGLE_ITEM)
    {
        setToggle(new ToggleSwitch(1));
    }

    ToggleItem::~ToggleItem()
    {
        delete _toggle;
    }

    void ToggleItem::onDraw()
    {
        if (!_is_changed)
        {
            if (_img)
                _img->onDraw();
            if (_label)
                _label->onDraw();
            if (_toggle)
                _toggle->onDraw();
            return;
        }

        _is_changed = false;

        if (!_is_transparent)
            clear();

        uint8_t img_width{0};
        if (_img)
        {
            _img->setParent(this);
            img_width = _img->getWidth() + ITEM_PADDING;
            _img->setPos(ITEM_PADDING, (_height - _img->getHeight()) * 0.5);

            uint16_t bk_color = _back_color;

            if (_has_focus && _need_change_back)
                _img->setBackColor(_focus_back_color);

            _img->setBackColor(bk_color);

            _img->onDraw();
        }

        _toggle->setPos(_width - ITEM_PADDING - _toggle->getWidth(), (_height - _toggle->getHeight()) * 0.5);
        _toggle->onDraw();

        _label->setHeight(_height - 2);
        _label->setPos(img_width + ITEM_PADDING, 1);
        _label->setWidth(_width - ITEM_PADDING * 2 - img_width - (_toggle->getWidth() + ITEM_PADDING));

        if (_has_focus)
            _label->setFocus();
        else
            _label->removeFocus();

        _label->onDraw();
    }

    ToggleItem *ToggleItem::clone(uint16_t id) const
    {
        try
        {
            ToggleItem *cln = new ToggleItem(id);
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

            if (_label)
                cln->setLbl(_label->clone(_label->getID()));

            if (_toggle)
                cln->setToggle(_toggle->clone(_toggle->getID()));

            return cln;
        }
        catch (const std::bad_alloc &e)
        {

            log_e("%s", e.what());
            esp_restart();
        }
    }

    void ToggleItem::setToggle(ToggleSwitch *togg_switch_ptr)
    {
        if (!togg_switch_ptr)
        {
            log_e("ToggleSwitch не може бути null");
            esp_restart();
        }

        if (togg_switch_ptr != _toggle)
        {
            delete _toggle;
            _toggle = togg_switch_ptr;
        }

        _is_changed = true;

        _toggle->setParent(this);
        _toggle->setChangingBorder(false);
    }

    void ToggleItem::on()
    {
        if (_toggle)
            _toggle->on();
    }

    void ToggleItem::off()
    {
        if (_toggle)
            _toggle->off();
    }

    bool ToggleItem::isOn() const
    {
        if (!_toggle)
            return false;

        return _toggle->isOn();
    }
}