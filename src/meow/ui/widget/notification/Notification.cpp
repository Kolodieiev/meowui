#include "Notification.h"

namespace meow
{

    Notification::Notification(uint16_t widget_ID, GraphicsDriver &display) : IWidget(widget_ID, display) {}

    Notification::~Notification()
    {
        delete _title_lbl;
        delete _msg_lbl;
        delete _left_lbl;
        delete _right_lbl;
    }

    void Notification::setLabels(Label *title, Label *msg, Label *left_btn, Label *right_btn)
    {
        if (!title || !msg || !left_btn || !right_btn)
        {
            log_e("Спроба передати NULL-об'єкт");
            esp_restart();
        }

        delete _title_lbl;
        delete _msg_lbl;
        delete _left_lbl;
        delete _right_lbl;

        _title_lbl = title;
        _msg_lbl = msg;
        _left_lbl = left_btn;
        _right_lbl = right_btn;

        _title_lbl->setTicker(true);
        _title_lbl->setParent(this);
        _title_lbl->setPos(2, 2);

        _msg_lbl->setMultiline(true);
        _msg_lbl->setPos(2, title->getHeight() + 4);

        _title_lbl->setParent(this);
        _msg_lbl->setParent(this);
        _left_lbl->setParent(this);
        _right_lbl->setParent(this);

        _height = _title_lbl->getHeight() + _msg_lbl->getHeight() + _left_lbl->getHeight() + 8;
    }

    void Notification::onDraw()
    {
        if (_is_hidden)
            return;

        if (!_is_transparent)
            clear();

        _title_lbl->setWidth(_width - 4);
        _msg_lbl->setWidth(_width - 4);
        _left_lbl->setWidth((float)(_width - 4) * 0.5);
        _right_lbl->setWidth((float)(_width - 4) * 0.5);

        _left_lbl->setPos(2, _msg_lbl->getYPos() + _msg_lbl->getHeight() + 2);
        _right_lbl->setPos(_left_lbl->getWidth() + 4, _msg_lbl->getYPos() + _msg_lbl->getHeight() + 2);

        _title_lbl->forcedDraw();
        _msg_lbl->forcedDraw();
        _left_lbl->forcedDraw();
        _right_lbl->forcedDraw();
    }

}