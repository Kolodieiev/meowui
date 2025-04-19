#include "Notification.h"

namespace meow
{

    Notification::Notification(uint16_t widget_ID, GraphicsDriver &display) : IWidget(widget_ID, display, TYPE_ID_NOTIFICATION) {}

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

        //
        _title_lbl->setAlign(ALIGN_CENTER);
        _title_lbl->setGravity(GRAVITY_CENTER);

        _left_lbl->setAlign(ALIGN_CENTER);
        _left_lbl->setGravity(GRAVITY_CENTER);

        _right_lbl->setAlign(ALIGN_CENTER);
        _right_lbl->setGravity(GRAVITY_CENTER);
        //
        _title_lbl->setTicker(true);
        _msg_lbl->setMultiline(true);
        _msg_lbl->setTextOffset(5);
        //
        uint16_t title_padding = 8;
        //
        _title_lbl->setHeight(_title_lbl->getHeight() + title_padding);
        _left_lbl->setHeight(_left_lbl->getHeight() + title_padding);
        _right_lbl->setHeight(_left_lbl->getHeight());
        _msg_lbl->setHeight(_display.height() - _h_margin * 2 - _title_lbl->getHeight() - _left_lbl->getHeight());
        //
        _title_lbl->setWidth(_display.width() - _w_margin * 2);
        _msg_lbl->setWidth(_title_lbl->getWidth());
        _left_lbl->setWidth((float)(_title_lbl->getWidth()) * 0.5);
        _right_lbl->setWidth(_left_lbl->getWidth());
        //
        _title_lbl->setPos(_w_margin, _h_margin);
        _msg_lbl->setPos(_w_margin, _title_lbl->getYPos() + _title_lbl->getHeight());
        _left_lbl->setPos(_w_margin, _msg_lbl->getYPos() + _msg_lbl->getHeight());
        _right_lbl->setPos(_left_lbl->getXPos() + _left_lbl->getWidth(), _left_lbl->getYPos());
    }

    void Notification::onDraw()
    {
        _title_lbl->forcedDraw();
        _msg_lbl->forcedDraw();
        _left_lbl->forcedDraw();
        _right_lbl->forcedDraw();
    }
}