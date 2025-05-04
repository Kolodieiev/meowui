#pragma once
#include <Arduino.h>
#include "../IWidget.h"
#include "../text/Label.h"

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
         * @brief Встановлює вказівники на віджети текстових міток, які будуть використовуватися у повідомленні.
         * Пам'ять, зайнята віджетами, буде автоматично очищена в деструкторі Notification.
         *
         * @param title Вказівник на віджет, що буде використовуватися для заголовка повідомлення.
         * @param msg Вказівник на віджет, що буде містити текст повідомлення.
         * @param left_btn Вказівник на віджет, що буде містити текст для лівої кнопки.
         * @param right_btn Вказівник на віджет, що буде містити текст для правої кнопки.
         */
        void setLabels(Label *title, Label *msg, Label *left_btn, Label *right_btn);

        /**
         * @brief Повертає вказівник на текстову мітку заголовка повідомлення.
         *
         * @return Label*
         */
        Label *getTitleLbl() const { return _title_lbl; }

        /**
         * @brief Повертає вказівник на текстову мітку тіла повідомлення.
         *
         * @return Label*
         */
        Label *getMsgLbl() const { return _msg_lbl; }

        /**
         * @brief Повертає вказівник на текстову мітку лівої кнопки повідомлення.
         *
         * @return Label*
         */
        Label *getLeftBtnLbl() const { return _left_lbl; }

        /**
         * @brief Повертає вказівник на текстову мітку правої кнопки повідомлення.
         *
         * @return Label*
         */
        Label *getRightBtnLbl() const { return _right_lbl; }

        /**
         * @brief Встановлює відступ для віджета від країв дисплею по горизонталі.
         * Значення не коригується, якщо відступ перевищує ширину дисплею.
         *
         * @param margin Значення відступу.
         */
        void setWMargin(uint16_t margin) { _w_margin = margin; }

        /**
         * @brief Встановлює відступ для віджета від країв дисплею по вертикалі.
         * Значення не коригується, якщо відступ перевищує висоту дисплею.
         *
         * @param margin Значення відступу.
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