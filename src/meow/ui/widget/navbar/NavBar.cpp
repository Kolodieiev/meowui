#include "NavBar.h"

namespace meow
{
    NavBar::NavBar(uint16_t widget_ID, GraphicsDriver &display) : IWidget(widget_ID, display) {}

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
            NavBar *clone = new NavBar(*this);
            clone->_id = id;

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