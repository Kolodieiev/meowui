#pragma GCC optimize("O3")

#include "IWidget.h"

namespace meow
{
    IWidget::IWidget(uint16_t widget_ID, GraphicsDriver &display, IWidget::TypeID type_ID, bool is_container) : _id{widget_ID},
                                                                                                                _display{display},
                                                                                                                _type_ID{type_ID},
                                                                                                                _is_container{is_container}

    {
        if (widget_ID == 0)
        {
            log_e("Ідентифікатор віджета повинен бути більшим за 0");
            esp_restart();
        }
    }

    IWidget::~IWidget() {}

    void IWidget::forcedDraw()
    {
        _is_changed = true;
        onDraw();
    }

    void IWidget::setPos(uint16_t x, uint16_t y)
    {
        _x_pos = x;
        _y_pos = y;
        _is_changed = true;
    }

    void IWidget::setHeight(uint16_t height)
    {
        _height = height;
        _is_changed = true;
    }

    void IWidget::setWidth(uint16_t width)
    {
        _width = width;
        _is_changed = true;
    }

    void IWidget::setBackColor(uint16_t back_color)
    {
        _back_color = back_color;
        _is_changed = true;
    }

    void IWidget::setParent(IWidget *parent)
    {
        _parent = parent;
        _is_changed = true;
    }

    void IWidget::setCornerRadius(uint8_t radius)
    {
        _corner_radius = radius;
        _is_changed = true;
    }

    void IWidget::setBorder(bool state)
    {
        _has_border = state;
        _is_changed = true;
    }

    void IWidget::setBorderColor(uint16_t color)
    {
        _border_color = color;
        _is_changed = true;
    }

    void IWidget::clear()
    {
        uint16_t x_offset{0};
        uint16_t y_offset{0};

        if (_parent)
        {
            x_offset = _parent->getXPos();
            y_offset = _parent->getYPos();
        }

        if (_corner_radius == 0)
        {
            _display.fillRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _back_color);

            if (_has_border)
                _display.drawRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _border_color);
        }
        else
        {
            _display.fillRoundRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _corner_radius, _back_color);

            if (_has_border)
            {
                _display.drawRoundRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _corner_radius, _border_color);
            }
            else if (_need_clear_border)
            {
                _need_clear_border = false;
                _display.drawRoundRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _corner_radius, _back_color);
            }
        }
    }

    void IWidget::hide()
    {
        uint16_t back_color{0};
        uint16_t x_offset{0};
        uint16_t y_offset{0};

        if (_parent)
        {
            x_offset = _parent->getXPos();
            y_offset = _parent->getYPos();
            back_color = _parent->getBackColor();
        }

        if (_corner_radius == 0)
            _display.fillRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, back_color);
        else
        {
            if (_has_border || _need_clear_border)
            {
                _need_clear_border = false;
                _display.drawRoundRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _corner_radius, back_color);
            }
            _display.fillRoundRect(_x_pos + x_offset, _y_pos + y_offset, _width, _height, _corner_radius, back_color);
        }
    }

    uint16_t IWidget::getXPos() const
    {
        if (_parent)
            return _parent->getXPos() + _x_pos;
        else
            return _x_pos;
    }

    uint16_t IWidget::getYPos() const
    {
        if (_parent)
            return _parent->getYPos() + _y_pos;
        else
            return _y_pos;
    }

    void IWidget::setChangingBorder(bool state)
    {
        _need_change_border = state;
        _is_changed = true;
    }

    void IWidget::setChangingBack(bool state)
    {
        _need_change_back = state;
        _is_changed = true;
    }

    void IWidget::setFocusBorderColor(uint16_t color)
    {
        _focus_border_color = color;
        _is_changed = true;
    }

    void IWidget::setFocusBackColor(uint16_t color)
    {
        _focus_back_color = color;
        _is_changed = true;
    }

    void IWidget::setFocus()
    {
        if (_has_focus)
            return;

        _is_changed = true;
        _has_focus = true;

        if (_need_change_border)
        {
            _old_border_state = _has_border;
            _has_border = true;

            _old_border_color = _border_color;
            _border_color = _focus_border_color;
        }

        if (_need_change_back)
        {
            _old_back_color = _back_color;
            _back_color = _focus_back_color;
        }
    }

    void IWidget::removeFocus()
    {
        if (!_has_focus)
            return;

        _is_changed = true;
        _has_focus = false;

        if (_need_change_border)
        {
            _need_clear_border = _has_border;
            _has_border = _old_border_state;
            _border_color = _old_border_color;
        }

        if (_need_change_back)
            _back_color = _old_back_color;
    }

    void IWidget::setVisibility(Visibility value)
    {
        _visibility = value;
        _is_changed = true;
    }

    bool IWidget::hasIntersectWithCoords(uint16_t x, uint16_t y)
    {
        if (_parent)
            return (x > _parent->getXPos() + _x_pos && x < _parent->getXPos() + _x_pos + _width) &&
                   (y > _parent->getYPos() + _y_pos && y < _parent->getYPos() + _y_pos + _height);
        else
            return (x > _x_pos && x < _x_pos + _width) && (y > _y_pos && y < _y_pos + _height);
    }
}