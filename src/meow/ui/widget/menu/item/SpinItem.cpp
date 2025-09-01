#pragma GCC optimize("O3")
#include "SpinItem.h"

namespace meow
{
    SpinItem::SpinItem(uint16_t widget_ID) : MenuItem(widget_ID, TYPE_ID_SPIN_ITEM)
    {
        setSpin(new SpinBox(1));
    }

    SpinItem::~SpinItem()
    {
        delete _spinbox;
    }

    void SpinItem::onDraw()
    {
        if (!_is_changed)
        {
            if (_img)
                _img->onDraw();
            _label->onDraw();
            _spinbox->onDraw();
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

        _spinbox->setHeight(_spinbox->getCharHgt() + 4);
        _spinbox->setPos(_width - ITEM_PADDING - _spinbox->getWidth(), (_height - _spinbox->getHeight()) * 0.5);
        _spinbox->onDraw();

        _label->setHeight(_height - 4);
        _label->setPos(img_width + ITEM_PADDING, 1);
        _label->setWidth(_width - ITEM_PADDING * 2 - img_width - (_spinbox->getWidth() + ITEM_PADDING));

        if (_has_focus)
            _label->setFocus();
        else
            _label->removeFocus();

        _label->onDraw();
    }

    SpinItem *SpinItem::clone(uint16_t id) const
    {
        try
        {
            SpinItem *cln = new SpinItem(id);
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
            cln->setSpin(_spinbox->clone(_spinbox->getID()));

            return cln;
        }
        catch (const std::bad_alloc &e)
        {

            log_e("%s", e.what());
            esp_restart();
        }
    }

    void SpinItem::setSpin(SpinBox *spinbox_ptr)
    {
        if (!spinbox_ptr)
        {
            log_e("SpinBox не може бути null");
            esp_restart();
        }

        if (spinbox_ptr == _spinbox)
            return;

        delete _spinbox;
        _spinbox = spinbox_ptr;

        _is_changed = true;

        _spinbox->setParent(this);
        _spinbox->setChangingBorder(false);
    }

    void SpinItem::up()
    {
        _spinbox->up();
    }

    void SpinItem::down()
    {
        _spinbox->down();
    }

    void SpinItem::setMin(float min)
    {
        _spinbox->setMin(min);
    }

    void SpinItem::setMax(float max)
    {
        _spinbox->setMax(max);
    }

    void SpinItem::setValue(float value)
    {
        _spinbox->setValue(value);
    }

    float SpinItem::getValue() const
    {
        return _spinbox->getValue();
    }
}