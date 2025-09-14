#pragma GCC optimize("O3")
#include "TextBox.h"

namespace meow
{
    TextBox::TextBox(uint16_t widget_ID, IWidget::TypeID type_ID) : Label(widget_ID, type_ID)
    {
        _text_gravity = GRAVITY_CENTER;
        _back_color = TFT_WHITE;
        _text_color = TFT_BLACK;
    }

    TextBox *TextBox::clone(uint16_t id) const
    {
        try
        {
            TextBox *cln = new TextBox(id);
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

            cln->_is_multiline = _is_multiline;
            cln->setText(_text);
            cln->_text_size = _text_size;
            cln->_text_color = _text_color;
            cln->_font_ID = _font_ID;
            cln->_text_offset = _text_offset;
            cln->_text_gravity = _text_gravity;
            cln->_text_alignment = _text_alignment;
            cln->_temp_width = _temp_width;

            if (_back_img)
            {
                cln->_back_img = _back_img->clone(_back_img->getID());
            }

            cln->_type = _type;

            return cln;
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
                uint32_t txt_len = calcRealStrLen(_text);

                String pwd_str = "";

                for (uint32_t i = 0; i < txt_len; ++i)
                    pwd_str += "*";

                _display.drawString(pwd_str, _x_pos + x_offset + txt_x_pos, _y_pos + y_offset + txtYPos);
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
                uint32_t txt_len = calcRealStrLen(sub_str);

                String pwd_str = "";

                for (uint32_t i = 0; i < txt_len; ++i)
                    pwd_str += "*";

                _display.drawString(pwd_str, _x_pos + x_offset + txt_x_pos, _y_pos + y_offset + txtYPos);
            }
        }
    }

}