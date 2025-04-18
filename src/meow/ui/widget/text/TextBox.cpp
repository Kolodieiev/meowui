#pragma GCC optimize("O3")
#include "TextBox.h"

namespace meow
{
    TextBox::TextBox(uint16_t widget_ID, GraphicsDriver &display, IWidget::TypeID class_ID) : Label(widget_ID,
                                                                                                     display,
                                                                                                     class_ID == TYPE_ID_UNKNOWN ? TYPE_ID_TEXTBOX : class_ID)
    {
        _text_gravity = GRAVITY_CENTER;
        _back_color = TFT_WHITE;
        _text_color = TFT_BLACK;
    }

    TextBox *TextBox::clone(uint16_t id) const
    {
        try
        {
            TextBox *clone = new TextBox(id, _display);
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

            clone->_is_multiline = _is_multiline;
            clone->setText(_text);
            clone->_text_size = _text_size;
            clone->_text_color = _text_color;
            clone->_font_ID = _font_ID;
            clone->_text_offset = _text_offset;
            clone->_text_gravity = _text_gravity;
            clone->_text_alignment = _text_alignment;
            clone->_is_ticker = _is_ticker;
            clone->_temp_is_ticker = _temp_is_ticker;
            clone->_is_ticker_in_focus = _is_ticker_in_focus;
            clone->_temp_is_ticker_in_focus = _temp_is_ticker_in_focus;
            clone->_temp_width = _temp_width;

            if (_back_img)
            {
                clone->_back_img = _back_img->clone(_back_img->getID());
            }

            clone->_type = _type;

            return clone;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }

    bool TextBox::removeLastChar()
    {
        if (_text.isEmpty())
            return false;

        _text = getSubStr(_text, 0, _text_len - 1);
        _text_len = calcRealStrLen(_text);

        _is_changed = true;
        return true;
    }

    void TextBox::addChars(const char *ch)
    {
        _text += ch;
        _text_len = calcRealStrLen(_text);
        _is_changed = true;
    }

    uint16_t TextBox::getFitStr(String &ret_str) const
    {
        uint16_t first_char_pos{1};

        const char *ch_str = _text.c_str();

        while (first_char_pos < _text_len - 1)
        {
            uint16_t pix_num = calcTextPixels(first_char_pos);

            if (pix_num < _width)
            {
                ret_str = getSubStr(ch_str, first_char_pos, calcRealStrLen(ch_str) - 1);
                return pix_num;
            }

            first_char_pos++;
        }

        return 0;
    }

    void TextBox::onDraw()
    {
        if (!_is_changed)
            return;

        _is_changed = false;

        clear();

        _display.setTextFont(_font_ID);
        _display.setTextSize(_text_size);
        _display.setTextColor(_text_color);

        uint16_t txtYPos = calcYStrOffset();
        uint16_t str_pix_num = calcTextPixels();

        uint16_t x_offset{0};
        uint16_t y_offset{0};

        if (_parent)
        {
            x_offset = _parent->getXPos();
            y_offset = _parent->getYPos();
        }

        if (str_pix_num + _text_offset < _width)
        {
            uint16_t txt_x_pos = calcXStrOffset(str_pix_num);

            if (_type == TYPE_TEXT)
            {
                _display.drawString(_text, _x_pos + x_offset + txt_x_pos, _y_pos + y_offset + txtYPos);
            }
            else // pwd
            {
                uint16_t txtLen = calcRealStrLen(_text);

                String pwdStr = "";

                for (uint16_t i; i < txtLen; ++i)
                    pwdStr += "*";

                _display.drawString(pwdStr, _x_pos + x_offset + txt_x_pos, _y_pos + y_offset + txtYPos);
            }
        }
        else
        {
            String sub_str;
            uint16_t sub_str_pix_num = getFitStr(sub_str);
            uint16_t txt_x_pos = calcXStrOffset(sub_str_pix_num);

            if (_type == TYPE_TEXT)
            {
                _display.drawString(sub_str, _x_pos + x_offset + txt_x_pos, _y_pos + y_offset + txtYPos);
            }
            else // pwd
            {
                uint16_t txtLen = calcRealStrLen(sub_str);

                String pwdStr = "";

                for (uint16_t i; i < txtLen; ++i)
                    pwdStr += "*";

                _display.drawString(pwdStr, _x_pos + x_offset + txt_x_pos, _y_pos + y_offset + txtYPos);
            }
        }
    }

}