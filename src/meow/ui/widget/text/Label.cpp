#pragma GCC optimize("O3")
#include "Label.h"
namespace meow
{
    Label::Label(uint16_t widget_ID, GraphicsDriver &display, IWidget::TypeID type_ID) : IWidget(widget_ID,
                                                                                                 display,
                                                                                                 type_ID == TYPE_ID_UNKNOWN ? TYPE_ID_LABEL : type_ID)
    {
        _height = chr_hgt_font2;
    }

    Label *Label::clone(uint16_t id) const
    {
        try
        {
            Label *clone = new Label(id, _display);
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

            return clone;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }

    void Label::initWidthToFit(uint16_t add_width_value)
    {
        _width = calcTextPixels();
        _width += 2 + add_width_value + _text_offset;

        _is_changed = true;
    }

    void Label::updateWidthToFit(uint16_t add_width_value)
    {
        _temp_width = calcTextPixels();
        _temp_width += 2 + add_width_value + _text_offset;

        _is_changed = true;
    }

    void Label::setText(const String &text)
    {
        try
        {
            _text = text;
            _text_len = calcRealStrLen(_text);
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            _text = "err";
            _text_len = 0;
        }

        _first_draw_char_pos = 0;
        _is_changed = true;
    }

    void Label::setTextSize(uint8_t size)
    {
        _text_size = size;

        uint16_t text_h = _font_ID == 2 ? chr_hgt_font2 * size + 2 : chr_hgt_font4 * size + 2;

        if (_height < text_h)
            _height = text_h;

        _is_changed = true;
    }

    void Label::setTextColor(uint16_t textColor)
    {
        _text_color = textColor;
        _is_changed = true;
    }

    uint8_t Label::getCharHgt() const
    {
        if (_font_ID == 2)
            return chr_hgt_font2 * _text_size;
        else
            return chr_hgt_font4 * _text_size;
    }

    void Label::setBackImg(Image *back_img)
    {
        _back_img = back_img;
        _is_changed = true;
    }

    void Label::setMultiline(bool state)
    {
        _is_multiline = state;
        _is_changed = true;
    }

    void Label::setTickerDelay(uint16_t delay)
    {
        _ticker_update_delay = delay;
        _is_changed = true;
    }

    void Label::setTicker(bool state)
    {
        _is_ticker = state;
        _temp_is_ticker = state;

        if (!state)
            _first_draw_char_pos = 0;

        _is_changed = true;
    }

    void Label::setTickerInFocus(bool state)
    {
        _is_ticker_in_focus = state;
        _temp_is_ticker_in_focus = state;

        if (!state && _is_ticker)
            _first_draw_char_pos = 0;

        _is_changed = true;
    }

    void Label::setFontID(uint8_t font_ID)
    {
        if (font_ID != 2 && font_ID != 4)
            _font_ID = 2;
        else
            _font_ID = font_ID;

        uint8_t char_h = 0;

        if (_font_ID == 2)
            char_h = chr_hgt_font2;
        else if (_font_ID == 4)
            char_h = chr_hgt_font4;

        // інші шрифти не підтримуються

        if (char_h > 0)
            if (_height < char_h * _text_size + 2)
                _height = char_h * _text_size + 2;

        _is_changed = true;
    }

    void Label::setGravity(const Gravity gravity)
    {
        _text_gravity = gravity;
        _is_changed = true;
    }

    void Label::setAlign(const Alignment alignment)
    {
        _text_alignment = alignment;
        _is_changed = true;
    }

    void Label::setTextOffset(uint8_t offset)
    {
        _text_offset = offset;
        _is_changed = true;
    }

    uint16_t Label::calcXStrOffset(uint16_t str_pix_num) const
    {
        if (_width > str_pix_num + _text_offset)
        {
            if (_is_ticker)
                return _text_offset;

            switch (_text_alignment)
            {
            case ALIGN_START:
                return _text_offset;

            case ALIGN_CENTER:
                return (float)(_width - str_pix_num) / 2 + _text_offset;

            case ALIGN_END:
                return _width - str_pix_num + _text_offset;
            }
        }

        return 0;
    }

    uint16_t Label::calcYStrOffset() const
    {
        uint8_t char_height{0};

        if (_font_ID == 2)
            char_height = chr_hgt_font2 * _text_size;
        else if (_font_ID == 4)
            char_height = chr_hgt_font4 * _text_size;

        if (_height > char_height)
        {
            switch (_text_gravity)
            {
            case GRAVITY_TOP:
                return 0;

            case GRAVITY_CENTER:
                return (float)(_height - char_height) / 2;

            case GRAVITY_BOTTOM:
                return _height - char_height;
            }
        }

        return 0;
    }

    uint16_t Label::calcRealStrLen(const String &str) const
    {
        if (str.isEmpty())
            return 0;

        uint16_t length{0};

        for (char c : str)
        {
            if ((c & 0xC0) != 0x80)
                ++length;
        }
        return length;
    }

    uint16_t Label::calcTextPixels(uint16_t char_pos) const
    {
        if (_text.isEmpty())
            return 0;

        uint16_t len = _text.length();
        uint8_t *ch_str_p8 = (uint8_t *)_text.c_str();

        uint16_t byte_pos = charPosToByte(ch_str_p8, char_pos);

        uint16_t unicode;
        uint16_t pix_sum{0};

        const unsigned char *width_table;

        if (_font_ID == 2)
            width_table = widtbl_f2;
        else if (_font_ID == 4)
            width_table = widtbl_f4;
        else
        {
            log_e("Невідомий шрифт");
            esp_restart();
        }

        while (byte_pos < len)
        {
            unicode = utf8ToUnicode(ch_str_p8, byte_pos, len - byte_pos);
            unicode = getCharPos(unicode);
            pix_sum += (*(const unsigned char *)(width_table + unicode));
        }

        return pix_sum * _text_size;
    }

    uint16_t Label::charPosToByte(const uint8_t *buf, uint16_t char_pos) const
    {
        uint16_t char_num = 0;
        uint16_t i = 0;

        while (char_num < char_pos && buf[i] != '\0')
        {
            if ((buf[i] & 0x80) == 0x00)
                ++i;
            else if ((buf[i] & 0xE0) == 0xC0)
                i += 2;
            else if ((buf[i] & 0xF0) == 0xE0)
                i += 3;
            else if ((buf[i] & 0xF8) == 0xF0)
                i += 4;
            else
                ++i;

            ++char_num;
        }

        return i;
    }

    uint32_t Label::utf8ToUnicode(const uint8_t *buf, uint16_t &byte_pos, uint16_t remaining) const
    {
        uint32_t codepoint = 0;

        if ((buf[byte_pos] & 0x80) == 0x00)
        {
            // 1-byte sequence
            codepoint = buf[byte_pos];
            ++byte_pos;
        }
        else if ((buf[byte_pos] & 0xE0) == 0xC0 && remaining > 1)
        {
            // 2-byte sequence
            codepoint = (buf[byte_pos] & 0x1F) << 6;
            ++byte_pos;
            codepoint |= (buf[byte_pos] & 0x3F);
            ++byte_pos;
        }
        else if ((buf[byte_pos] & 0xF0) == 0xE0 && remaining > 2)
        {
            // 3-byte sequence
            codepoint = (buf[byte_pos] & 0x0F) << 12;
            ++byte_pos;
            codepoint |= (buf[byte_pos] & 0x3F) << 6;
            ++byte_pos;
            codepoint |= (buf[byte_pos] & 0x3F);
            ++byte_pos;
        }
        else if ((buf[byte_pos] & 0xF8) == 0xF0 && remaining > 3)
        {
            // 4-byte sequence
            codepoint = (buf[byte_pos] & 0x07) << 18;
            ++byte_pos;
            codepoint |= (buf[byte_pos] & 0x3F) << 12;
            ++byte_pos;
            codepoint |= (buf[byte_pos] & 0x3F) << 6;
            ++byte_pos;
            codepoint |= (buf[byte_pos] & 0x3F);
            ++byte_pos;
        }
        else
            ++byte_pos;

        return codepoint;
    }

    uint16_t Label::getFitStr(String &ret_str, uint16_t start_pos) const
    {
        if (_text.isEmpty())
        {
            ret_str = "";
            return 0;
        }

        uint16_t len = _text.length();

        if (start_pos >= len)
        {
            ret_str = "";
            return 0;
        }

        const unsigned char *width_table;

        if (_font_ID == 2)
            width_table = widtbl_f2;
        else if (_font_ID == 4)
            width_table = widtbl_f4;
        else
        {
            log_e("Невідомий шрифт");
            esp_restart();
        }

        uint8_t *ch_str_p8 = (uint8_t *)_text.c_str();

        uint16_t byte_pos = charPosToByte(ch_str_p8, start_pos);

        uint32_t code;
        uint16_t pos;

        uint16_t pix_sum{0};
        uint16_t chars_counter{0};

        while (byte_pos < len)
        {
            code = utf8ToUnicode(ch_str_p8, byte_pos, len - byte_pos);

            if (!code)
                continue;

            pos = getCharPos(code);

            uint16_t char_w = (*(const unsigned char *)(width_table + pos)) * _text_size;

            if (pix_sum + char_w >= _width - _text_offset - 3)
                break;
            else
            {
                pix_sum += char_w;
                ++chars_counter;
            }
        }

        ret_str = getSubStr(_text, start_pos, chars_counter);

        return pix_sum;
    }

    String Label::getSubStr(const String &str, uint16_t start, uint16_t length) const
    {
        if (!length)
            return "";

        unsigned int c, i, ix, q;
        unsigned int min = -1, max = -1; //

        for (q = 0, i = 0, ix = str.length(); i < ix; ++i, q++)
        {
            if (q == start)
                min = i;

            if (q <= start + length || length == -1)
                max = i;

            c = (unsigned char)str[i];

            if (c >= 0 && c <= 127)
                i += 0;
            else if ((c & 0xE0) == 0xC0)
                ++i;
            else if ((c & 0xF0) == 0xE0)
                i += 2;
            else if ((c & 0xF8) == 0xF0)
                i += 3;
            else
                ++i; // invalid utf8
        }

        if (q <= start + length || length == -1)
            max = i;

        if (min == -1 || max == -1)
            return "";

        return str.substring(min, max);
    }

    uint16_t Label::getCharPos(uint32_t unicode) const
    {
        if (unicode > 127)
        {
            if (unicode > 1039 && unicode < 1104)
                unicode = unicode - 944;
            else
                switch (unicode)
                {
                case 1030:
                    unicode = 41;
                    break;
                case 1031:
                    unicode = 162;
                    break;
                case 1111:
                    unicode = 163;
                    break;
                case 1110:
                    unicode = 73;
                    break;
                case 1028:
                    unicode = 160;
                    break;
                case 1108:
                    unicode = 161;
                    break;
                case 1025:
                    unicode = 37;
                    break;
                case 1105:
                    unicode = 69;
                    break;
                case 1168:
                    unicode = 164;
                    break;
                case 1169:
                    unicode = 165;
                    break;
                case 171:
                case 187:
                    unicode = 2;
                    break;
                default:
                    unicode = CHRS_NMBR - 1;
                }
        }
        else if (unicode < 32)
            unicode = 0;
        else
            unicode -= 32;

        return unicode;
    }

    void Label::onDraw()
    {
        if (!_is_changed)
        {
            if (!_is_ticker && !(_is_ticker_in_focus && _has_focus) || _visibility == INVISIBLE)
                return;
            else if ((millis() - _last_time_ticker_update) < _ticker_update_delay)
                return;
            else
                _last_time_ticker_update = millis();
        }

        _is_changed = false;

        if (_visibility == INVISIBLE)
        {
            if (!_is_transparent)
                hide();
            return;
        }

        if (_temp_is_ticker_in_focus && !_has_focus)
            _first_draw_char_pos = 0;

        if (!_is_transparent)
            clear();

        if (_temp_width > 0)
        {
            _width = _temp_width;
            _temp_width = 0;
        }

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

        if (_back_img)
        {
            _back_img->setParent(this);
            _back_img->forcedDraw();
        }

        if (str_pix_num + _text_offset < _width)
        {
            // Якщо текст не потребує прокрутки, вимикаємо її для зменшення навантаження на МК.
            _is_ticker = false;
            _is_ticker_in_focus = false;

            uint16_t txt_x_pos = calcXStrOffset(str_pix_num);

            _display.drawString(_text, _x_pos + x_offset + txt_x_pos, _y_pos + y_offset + txtYPos);
        }
        else
        {
            String sub_str;
            uint16_t sub_str_pix_num{0};
            uint16_t txt_x_pos{0};

            if (!_is_multiline)
            {
                sub_str_pix_num = getFitStr(sub_str, _first_draw_char_pos);

                txt_x_pos = calcXStrOffset(sub_str_pix_num);

                if (_temp_is_ticker || (_temp_is_ticker_in_focus && _has_focus))
                {
                    if ((millis() - _last_time_ticker_update) > _ticker_update_delay)
                    {
                        if (_first_draw_char_pos == _text_len - 1 || sub_str_pix_num == 0)
                            _first_draw_char_pos = 0;
                        else
                            ++_first_draw_char_pos;
                        _last_time_ticker_update = millis();
                    }
                }

                _display.drawString(sub_str, _x_pos + x_offset + txt_x_pos, _y_pos + y_offset + txtYPos);
            }
            else
            {
                uint8_t font_height = _font_ID == 2 ? chr_hgt_font2 : chr_hgt_font4;
                uint8_t line_spacing = (float)font_height * _text_size * 0.15;

                _first_draw_char_pos = 0;
                _is_ticker = false;
                _is_ticker_in_focus = false;

                if (font_height * 2 + line_spacing < _height - 2) // Якщо можна помістити більше ніж один рядок тексту
                {
                    uint16_t y_pos{0};

                    while (y_pos + font_height + line_spacing < _height - 2)
                    {
                        sub_str_pix_num = getFitStr(sub_str, _first_draw_char_pos);

                        if (sub_str_pix_num > 0)
                        {
                            txt_x_pos = calcXStrOffset(sub_str_pix_num);
                            _display.drawString(sub_str, _x_pos + x_offset + txt_x_pos, _y_pos + y_offset + y_pos);

                            y_pos += font_height + line_spacing;
                            _first_draw_char_pos += calcRealStrLen(sub_str);
                        }
                        else
                            break;
                    }
                }
                else
                {
                    sub_str_pix_num = getFitStr(sub_str, _first_draw_char_pos);
                    txt_x_pos = calcXStrOffset(sub_str_pix_num);
                    _display.drawString(sub_str, _x_pos + x_offset + txt_x_pos, _y_pos + y_offset + txtYPos);
                }
            }
        }
    }

}