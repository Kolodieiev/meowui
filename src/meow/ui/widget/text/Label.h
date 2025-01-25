#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include "../IWidget.h"
#include "../image/Image.h"

namespace meow
{
    // TODO додати перенесення рядків по /n

    class Label : public IWidget
    {
    public:
        Label(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~Label() {};

        /**
         * @brief Викликає процедуру малювання віджета на дисплей.
         * Якщо віджет не було змінено, він автоматично пропустить перемальовування.
         * 
         */
        virtual void onDraw() override;

        /**
         * @brief Повертає вказівник на глибоку копію віджета.
         * 
         * @param id Ідентифікатор, який буде присвоєно новому віджету.
         * @return Label* 
         */
        virtual Label *clone(uint16_t id) const override;

        /**
         * @brief Автоматично розраховує початкову ширину віджета.
         * Викликайте цей метод тільки після того, як налаштували інші параметри для віджета.
         * Задає початкову ширину віджета таким чином, щоб вмістити текст + відступ.
         *
         * @param add_width_value Значення, що буде додане до ширини віджета.
         */
        void initWidthToFit(uint16_t add_width_value = 0);

        /**
         * @brief Автоматично розраховує та оновлює ширину віджета таким чином, щоб вмістити текст + відступи.
         * Викликайте цей метод тільки після того, як налаштували інші відповідні параметри віджета.
         * Допомогає коректно замальовувати попереднє місце свого розрташування.
         *
         * @param add_width_value Значення, що буде додане до ширини віджета.
         */
        void updateWidthToFit(uint16_t add_width_value = 0);

        /**
         * @brief Встановлює текст, що буде відображатися у віджеті.
         *
         * @param text
         */
        void setText(const String &text);

        /**
         * @brief Повертає текст, що зараз встановлено у віджеті.
         *
         * @return String
         */
        String getText() const
        {
            return _text;
        }

        /**
         * @brief Встановлює розмір тексту.
         *
         * @param size Розмір тексту. Значення може бути від 1 до 7.
         */
        void setTextSize(uint8_t size)
        {
            _text_size = size;

            uint16_t text_h = _font_ID == 2 ? chr_hgt_font2 * size + 2 : chr_hgt_font4 * size + 2;

            if (_height < text_h)
                _height = text_h;

            _is_changed = true;
        }

        /**
         * @brief Встановлює колір тексту.
         *
         * @param textColor
         */
        void setTextColor(uint16_t textColor)
        {
            _text_color = textColor;
            _is_changed = true;
        }

        /**
         * @brief Встановлює ідентифікатор шрифту тексту.
         * По замовченню встановлено шрифт з ID == 2.
         *
         * @param font_ID Ідентифіктор шрифту. Наразі може мати значення тільки 2 або 4.
         */
        void setFontID(uint8_t font_ID);

        /**
         * @brief Повертає ідентифікатор поточного шрифту.
         *
         * @return uint8_t
         */
        uint8_t getFontID() const { return _font_ID; }

        /**
         * @brief Встановлює положення тексту по вертикалі відносно віджета.
         *
         * @param gravity Може мати значення: GRAVITY_TOP / GRAVITY_CENTER / GRAVITY_BOTTOM.
         */
        void setGravity(const Gravity gravity)
        {
            _text_gravity = gravity;
            _is_changed = true;
        }

        /**
         * @brief Встановлює положення тексту по горизонталі відносно віджета.
         *
         * @param alignment Може мати значення: ALIGN_START / ALIGN_CENTER / ALIGN_END.
         */
        void setAlign(const Alignment alignment)
        {
            _text_alignment = alignment;
            _is_changed = true;
        }

        /**
         * @brief Встановлює зміщення позиції тексту (в пікселях) вправо від початку віджета.
         *
         * @param offset Значення зміщення тексту.
         */
        void setTextOffset(uint8_t offset)
        {
            _text_offset = offset;
            _is_changed = true;
        }

        /**
         * @brief Повертає кількість символів, що зберігається в данному віджеті.
         * Враховуються тільки символи в кодуванні UTF-8, а не байти.
         *
         * @return uint16_t
         */
        uint16_t getTextLength() const { return _text_len; }

        /**
         * @brief Встановлює прапор, який керує механізмом прокручування тексту,
         * якщо ширини віджета не достатньо для його відображення.
         *
         * @param state Якщо true - текст буде прокручуватися автоматично.
         */
        void setTicker(bool state);

        /**
         * @brief Повертає значення прапору, який керує механізмом прокручування тексту.
         *
         * @return true - Текст буде прокручуватися, якщо ширина віджета недостатньо, щоб вмістити його повністю.
         * @return false - Текст не буде прокручуватися.
         */
        bool isTicker() const { return _temp_is_ticker; }

        /**
         * @brief Встановлює прапор, який керує механізмом прокручування тексту коли віджет потрапляє у фокус,
         * при цьому його ширини не достатньо, щоб повністю відобразити текст.
         *
         * @param state Якщо true - текст буде прокручуватися при потраплянні віджета у фокус.
         */
        void setTickerInFocus(bool state);

        /**
         * @brief Повертає значення пропора, який керує механізмом прокручування тексту коли віджет потрапляє у фокус,
         * при цьому його ширини не достатньо, щоб повністю відобразити текст.
         *
         * @return true - Текст буде прокручуватися при потраплянні віджета у фокус.
         * @return false - Інакше.
         */
        bool isTickerInFocus() const { return _temp_is_ticker_in_focus; }

        /**
         * @brief Повертає висоту шрифта цього віджета в пікселях.
         *
         * @return uint8_t
         */
        uint8_t getCharHgt() const;

        /**
         * @brief Встановлює вказівник на повністю ініціалізований віджет зображення,
         * яке буде відображатися в якості фону текстової мітки.
         * Віджет зображення не буде видалено автоматично разом із текстовою міткою.
         *
         * @param back_img Вказівник на повністю ініціалізоване зображення.
         */
        void setBackImg(Image *back_img);

        /**
         * @brief Повертає вказівник на фонове зображення цього віджета.
         *
         * @return Image*
         */
        Image *getBackImg() const { return _back_img; }

        /**
         * @brief Встановлює стан прапора, який керує механізмом відображення тексту в декілька рядків.
         *
         * @param state
         * true - Текст буде відображатися в декільки рядків. Скидає стан прапору isTicker.
         * false - Текст буде відображатися в один рядок.
         */
        void setMultiline(bool state);

        /**
         * @brief Повертає стан прапора, який керує механізмом відображення тексту в декілька рядків.
         *
         * @return true - Текст може відображатися в декілька рядків, якщо не вміщується в один.
         * @return false - Текст відображається тільки в один рядок.
         */
        bool isMultiline() const { return _is_multiline; }

    protected:
        bool _is_multiline{false};
        uint16_t _text_len{0};
        String _text{""};
        uint8_t _text_size{1};
        uint16_t _text_color{0xFFFF};
        uint8_t _font_ID{2};
        uint8_t _text_offset{0};
        Gravity _text_gravity{GRAVITY_TOP};
        Alignment _text_alignment{ALIGN_START};

        bool _is_ticker{false};
        bool _temp_is_ticker{false};
        bool _is_ticker_in_focus{false};
        bool _temp_is_ticker_in_focus{false};

        unsigned long _last_time_ticker_update{0};
        const uint8_t TICKER_UPDATE_DELAY{160};

        uint16_t _temp_width{0};

        uint16_t _first_draw_char_pos{0};
        //
        Image *_back_img{nullptr};
        //
        uint16_t calcXStrOffset(uint16_t str_pix_num) const;
        uint16_t calcYStrOffset() const;
        uint16_t calcRealStrLen(const String &str) const;
        uint16_t getFitStr(String &ret_str, uint16_t start_pos = 0) const;
        String getSubStr(const String &str, uint16_t from, uint16_t length) const;
        uint16_t calcTextPixels(uint16_t char_pos = 0) const;
        uint16_t getCharPos(uint32_t unicode) const;

    private:
        uint16_t charPosToByte(const uint8_t *buf, uint16_t char_pos) const;
        uint32_t utf8ToUnicode(const uint8_t *buf, uint16_t &char_pos, uint16_t remaining) const;
    };

}