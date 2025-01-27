#pragma once
#include <Arduino.h>
#include "../IWidget.h"
#include "../text/Label.h"

namespace meow
{
    class Notification : public IWidget
    {

    public:
        Notification(uint16_t widget_ID, GraphicsDriver &display);
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
         * @brief Встановлює вказівники на віджети текстових міток, які будуть використовуватися у повідомленні.
         *
         * @param title Вказівник на віджет, що буде використовуватися для заголовка повідомлення.
         * @param msg Вказівник на віджет, що буде містити текст повідомлення.
         * @param left_btn Вказівник на віджет, що буде містити текст для лівої кнопки.
         * @param right_btn Вказівник на віджет, що буде містити текст для правої кнопки.
         */
        void setLabels(Label *title, Label *msg, Label *left_btn, Label *right_btn);

        /**
         * @brief Вмикає відображення віджета.
         *
         */
        void show() { _is_hidden = false; }

        /**
         * @brief Вимикає відображення віджета.
         *
         */
        void hide() { _is_hidden = true; }

        /**
         * @brief Повертає поточний стан прапору, який відповідає за механізм відображення віджета.
         *
         * @return true - Якщо віджет зараз приховано.
         * @return false - Якщо віджет зараз відображається.
         */
        bool isHidden() const { return _is_hidden; }

    private:
        using IWidget::getFocusBackColor;
        using IWidget::getFocusBorderColor;
        using IWidget::setFocus;
        using IWidget::setFocusBackColor;
        using IWidget::setFocusBorderColor;
        using IWidget::setHeight;
        using IWidget::setVisibility;

        Label *_title_lbl{nullptr};
        Label *_msg_lbl{nullptr};
        Label *_left_lbl{nullptr};
        Label *_right_lbl{nullptr};

        bool _is_hidden{true};
    };
}