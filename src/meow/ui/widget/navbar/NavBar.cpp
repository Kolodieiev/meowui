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
            NavBar *cln = new NavBar(id);
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

            if (_first)
            {
                cln->setWidgets(
                    _first->clone(_first->getID()),
                    _middle->clone(_middle->getID()),
                    _last->clone(_last->getID()));
            }

            return cln;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }
}