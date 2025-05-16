#pragma once
#include <Arduino.h>
#include "../IWidget.h"
#include "../text/Label.h"

// TODO замінити завантаження label на прості виклики, типу set text

namespace meow
{
    class Notification final : public IWidget
    {

    public:
        Notification(uint16_t widget_ID);
        virtual ~Notification();

        /**
         * @brief Викликає процедуру малювання віджета на дисплей.
         * Якщо віджет не було змінено, він автоматично пропустить перемальовування.
         *
         */
        virtual void onDraw() override;

        /**
         * @brief STUB! Не викликай!
         */
        Notification *clone(uint16_t id) const override
        {
            log_e("Клонування цього віджета неможливе");
            esp_restart();
            return nullptr;
        }

        /**
         * @brief Повертає ідентифікатор типу.
         * Використовується в системі приведення типу.
         *
         * @return constexpr TypeID
         */
        static constexpr TypeID staticType() { return TypeID::TYPE_ID_NOTIFICATION; }

        /**
         * @brief Встановлює ідентифікатор шрифту заголовка та кнопок.
         *
         * @param font_id
         */
        void setInfoFontID(uint16_t font_id);

        /**
         * @brief Встановлює розмір шрифту заголовка та кнопок.
         *
         * @param size
         */
        void setInfoTextSize(uint16_t size);

        /**
         * @brief Встановлює текст заголовка повідомлення.
         *
         * @param str
         */
        void setTitleText(const String &str);

        /**
         * @brief Встановлює колір фону заголовка.
         *
         * @param color
         */
        void setTitleBackColor(uint16_t color);

        /**
         * @brief Встановлює колір тексту заголовка.
         *
         * @param color
         */
        void setTitleTextColor(uint16_t color);

        /**
         * @brief Встановлює текст лівої кнопки повідомлення.
         *
         * @param str
         */
        void setLeftText(const String &str);

        /**
         * @brief Встановлює колір фону лівої кнопки повідомлення.
         *
         * @param color
         */
        void setLeftBackColor(uint16_t color);

        /**
         * @brief Встановлює колір тексту лівої кнопки повідомлення.
         *
         * @param color
         */
        void setLeftTextColor(uint16_t color);

        /**
         * @brief Встановлює текст правої кнопки повідомлення.
         *
         * @param str
         */
        void setRightText(const String &str);

        /**
         * @brief Встановлює колір фону правої кнопки повідомлення.
         *
         * @param color
         */
        void setRightBackColor(uint16_t color);

        /**
         * @brief Встановлює колір тексту правої кнопки повідомлення.
         *
         * @param color
         */
        void setRightTextColor(uint16_t color);

        /**
         * @brief Встановлює текст тіла повідомлення.
         *
         * @param str
         */
        void setMsgText(const String &str);

        /**
         * @brief Встановлює колір фону тіла повідомлення.
         *
         * @param color
         */
        void setMsgBackColor(uint16_t color);

        /**
         * @brief Встановлює колір тексту тіла повідомлення.
         *
         * @param color
         */
        void setMsgTextColor(uint16_t color);

        /**
         * @brief Встановлює ідентифікатор шрифту тіла повідомлення.
         *
         * @param font_id
         */
        void setMsgFontID(uint16_t font_id);

        /**
         * @brief Встановлює розмір шрифту тіла повідомлення.
         *
         * @param size
         */
        void setMsgTextSize(uint16_t size);

        /**
         * @brief Встановлює відступ для віджета від країв дисплею по горизонталі.
         * Значення не коригується автоматично, якщо відступ перевищує ширину дисплею.
         *
         * @param margin
         */
        void setWMargin(uint16_t margin) { _w_margin = margin; }

        /**
         * @brief Встановлює відступ для віджета від країв дисплею по вертикалі.
         * Значення не коригується автоматично, якщо відступ перевищує висоту дисплею.
         *
         * @param margin
         */
        void setHMargin(uint16_t margin) { _h_margin = margin; }

    private:
        using IWidget::getFocusBackColor;
        using IWidget::removeFocus;
        using IWidget::setBorder;
        using IWidget::setCornerRadius;
        using IWidget::setFocus;
        using IWidget::setFocusBackColor;
        using IWidget::setHeight;
        using IWidget::setVisibility;

        Label *_title_lbl{nullptr};
        Label *_msg_lbl{nullptr};
        Label *_left_lbl{nullptr};
        Label *_right_lbl{nullptr};

        uint16_t _w_margin{30};
        uint16_t _h_margin{30};
    };
}