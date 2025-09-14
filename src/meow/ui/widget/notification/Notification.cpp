#include "Notification.h"

namespace meow
{
    Notification::Notification(uint16_t widget_ID) : IWidget(widget_ID, TYPE_ID_NOTIFICATION)
    {
        setBorderColor(TFT_ORANGE);
        setBorder(true);

        _title_lbl = new Label(1);
        _msg_lbl = new Label(1);
        _left_lbl = new Label(1);
        _right_lbl = new Label(1);

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
        _width = _display.width() - _w_margin * 2;
        _title_lbl->setWidth(_width - 4);
        _msg_lbl->setWidth(_title_lbl->getWidth());
        _left_lbl->setWidth((float)(_title_lbl->getWidth()) * 0.5);
        _right_lbl->setWidth(_left_lbl->getWidth());
        //
        _x_pos = _w_margin;
        _y_pos = _h_margin;
        _title_lbl->setPos(_w_margin + 2, _h_margin + 2);
        _msg_lbl->setPos(_title_lbl->getXPos(), _title_lbl->getYPos() + _title_lbl->getHeight());
        _left_lbl->setPos(_title_lbl->getXPos(), _msg_lbl->getYPos() + _msg_lbl->getHeight());
        _right_lbl->setPos(_left_lbl->getXPos() + _left_lbl->getWidth(), _left_lbl->getYPos());

        _height = _title_lbl->getHeight() + _msg_lbl->getHeight() + _right_lbl->getHeight() + 4;
    }

    Notification::~Notification()
    {
        delete _title_lbl;
        delete _msg_lbl;
        delete _left_lbl;
        delete _right_lbl;
    }

    void Notification::onDraw()
    {
        clear();

        _title_lbl->forcedDraw();
        _msg_lbl->forcedDraw();
        _left_lbl->forcedDraw();
        _right_lbl->forcedDraw();
    }

    void Notification::setInfoFontID(uint16_t font_id)
    {
        _title_lbl->setFontID(font_id);
        _left_lbl->setFontID(font_id);
        _right_lbl->setFontID(font_id);
    }

    void Notification::setInfoTextSize(uint16_t size)
    {
        _title_lbl->setTextSize(size);
        _left_lbl->setTextSize(size);
        _right_lbl->setTextSize(size);
    }

    void Notification::setTitleText(const String &str)
    {
        _title_lbl->setText(str);
    }

    void Notification::setTitleBackColor(uint16_t color)
    {
        _title_lbl->setBackColor(color);
    }

    void Notification::setTitleTextColor(uint16_t color)
    {
        _title_lbl->setTextColor(color);
    }

    void Notification::setMsgText(const String &str)
    {
        _msg_lbl->setText(str);
    }

    void Notification::setMsgBackColor(uint16_t color)
    {
        _msg_lbl->setBackColor(color);
    }

    void Notification::setMsgTextColor(uint16_t color)
    {
        _msg_lbl->setTextColor(color);
    }

    void Notification::setMsgFontID(uint16_t font_id)
    {
        _msg_lbl->setFontID(font_id);
    }

    void Notification::setMsgTextSize(uint16_t size)
    {
        _msg_lbl->setTextSize(size);
    }

    void Notification::setLeftText(const String &str)
    {
        _left_lbl->setText(str);
    }

    void Notification::setLeftBackColor(uint16_t color)
    {
        _left_lbl->setBackColor(color);
    }

    void Notification::setLeftTextColor(uint16_t color)
    {
        _left_lbl->setTextColor(color);
    }

    void Notification::setRightText(const String &str)
    {
        _right_lbl->setText(str);
    }

    void Notification::setRightBackColor(uint16_t color)
    {
        _right_lbl->setBackColor(color);
    }

    void Notification::setRightTextColor(uint16_t color)
    {
        _right_lbl->setTextColor(color);
    }
}