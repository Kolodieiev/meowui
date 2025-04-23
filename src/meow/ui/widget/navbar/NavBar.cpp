#include "NavBar.h"

namespace meow
{
    NavBar::NavBar(uint16_t widget_ID) : IWidget(widget_ID, TYPE_ID_NAVBAR) {}

    NavBar::~NavBar()
    {
        delete _first;
        delete _middle;
        delete _last;
    }

    void NavBar::setWidgets(IWidget *first, IWidget *middle, IWidget *last)
    {
        if (!first || !middle || !last)
        {
            log_e("Спроба передати NULL-об'єкт.");
            esp_restart();
        }

        delete _first;
        delete _middle;
        delete _last;

        _first = first;
        _middle = middle;
        _last = last;

        _first->setParent(this);
        _middle->setParent(this);
        _last->setParent(this);

        _is_changed = true;
    }

    void NavBar::onDraw()
    {
        if (!_is_changed)
        {
            if (_visibility == INVISIBLE || !_first)
                return;

            _first->onDraw();
            _middle->onDraw();
            _last->onDraw();

            return;
        }

        _is_changed = false;

        if (_visibility == INVISIBLE)
        {
            hide();
            return;
        }

        clear();

        if (!_first)
            return;

        _first->setPos(0, 0);
        _first->setBackColor(_back_color);
        _first->setHeight(_height);
        _first->onDraw();

        _middle->setPos((_width - _middle->getWidth()) * 0.5, 0);
        _middle->setBackColor(_back_color);
        _middle->setHeight(_height);
        _middle->onDraw();

        _last->setPos(_width - _last->getWidth(), 0);
        _last->setBackColor(_back_color);
        _last->setHeight(_height);
        _last->onDraw();
    }

    NavBar *NavBar::clone(uint16_t id) const
    {
        try
        {
            NavBar *clone = new NavBar(id);
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

            if (_first)
            {
                clone->setWidgets(
                    _first->clone(_first->getID()),
                    _middle->clone(_middle->getID()),
                    _last->clone(_last->getID()));
            }

            return clone;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }
}