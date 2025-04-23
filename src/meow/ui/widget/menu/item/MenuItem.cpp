#pragma GCC optimize("O3")

#include "./MenuItem.h"

namespace meow
{
    MenuItem::MenuItem(uint16_t widget_ID, IWidget::TypeID type_ID) : IWidget(widget_ID,
                                                                              type_ID == TYPE_ID_UNKNOWN ? TYPE_ID_MENU_ITEM : type_ID) {}

    MenuItem::~MenuItem()
    {
        delete _label;
    }

    String MenuItem::getText() const
    {
        if (!_label)
        {
            log_e("Не встановлено Label");
            esp_restart();
        }
        return _label->getText();
    }

    void MenuItem::onDraw()
    {
        if (!_is_changed)
        {
            if (_ico)
                _ico->onDraw();
            if (_label)
                _label->onDraw();
            return;
        }

        _is_changed = false;

        if (!_is_transparent)
            clear();

        uint8_t img_width{0};

        if (_ico)
        {
            _ico->setParent(this);
            img_width = _ico->getWidth() + ITEM_PADDING;
            _ico->setPos(ITEM_PADDING, (_height - _ico->getHeight()) * 0.5);

            uint16_t bk_color = _back_color;

            if (_has_focus && _need_change_back)
                _ico->setBackColor(_focus_back_color);

            _ico->setBackColor(bk_color);

            _ico->onDraw();
        }

        if (_label)
        {
            _label->setHeight(_height - 2);
            _label->setPos(img_width + ITEM_PADDING, 1);
            _label->setWidth(_width - ITEM_PADDING * 2 - img_width);

            if (_has_focus)
                _label->setFocus();
            else
                _label->removeFocus();

            _label->onDraw();
        }
    }

    MenuItem *MenuItem::clone(uint16_t id) const
    {
        try
        {
            MenuItem *clone = new MenuItem(id);
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

            if (_ico)
                clone->setIco(_ico->clone(_ico->getID()));

            if (_label)
                clone->setLbl(_label->clone(_label->getID()));

            return clone;
        }
        catch (const std::bad_alloc &e)
        {

            log_e("%s", e.what());
            esp_restart();
        }
    }

    void MenuItem::setIco(Image *img_ptr)
    {
        delete _ico;

        _ico = img_ptr;
        _is_changed = true;
    }

    void MenuItem::setLbl(Label *lbl_ptr)
    {
        if (!lbl_ptr)
        {
            log_e("Label не може бути null");
            esp_restart();
        }

        delete _label;

        _label = lbl_ptr;
        _is_changed = true;

        _label->setParent(this);
        _label->setBorder(false);
        _label->setBackColor(_back_color);
        _label->setFocusBackColor(_focus_back_color);
        _label->setChangingBack(_need_change_back);
        _label->setChangingBorder(false);
        _label->setGravity(GRAVITY_CENTER);
        _label->setTransparency(_is_transparent);
    }
}