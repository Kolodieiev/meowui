#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../IWidgetContainer.h"
#include "./KeyboardRow.h"

namespace meow
{
    class Keyboard : public IWidgetContainer
    {
    public:
        Keyboard(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~Keyboard() {}

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
         * @return Keyboard*
         */
        virtual Keyboard *clone(uint16_t id) const override;

        /**
         * @brief Повертає ідентифікатор віджета кнопки, на якому встановлено фокус.
         *
         * @return uint16_t
         */
        uint16_t getCurrentBtnID() const;

        /**
         * @brief Повертає копію тексту, що міститься у віджеті кнопки, на якому встановлено фокус.
         *
         * @return String
         */
        String getCurrentBtnTxt() const;

        /**
         * @brief Переміщує фокус, якщо можливо, на віджет навпроти, що знаходиться на один рядок вище.
         * Інакше, якщо можливо, переміщує фокус на віджет у самому нижньому рядку розкладки.
         *
         */
        void focusUp();

        /**
         * @brief Переміщує фокус, якщо можливо, на віджет навпроти, що знаходиться на один рядок нище.
         * Інакше, якщо можливо, переміщує фокус на віджет у самому верхньому рядку розкладки.
         *
         */
        void focusDown();

        /**
         * @brief Переміщує фокус, якщо можливо, на віджет вліво, що знаходиться в цьому рядку.
         * Інакше, якщо можливо, переміщує фокус на крайній правий віджет в цьому ряду.
         *
         */
        void focusLeft();

        /**
         * @brief Переміщує фокус, якщо можливо, на віджет вправо, що знаходиться в цьому рядку.
         * Інакше, якщо можливо, переміщує фокус на крайній лівий віджет в цьому ряду.
         *
         */
        void focusRight();

    private:
        uint16_t _cur_focus_row_pos{0};

        bool _first_drawing{true};

        KeyboardRow *getFocusRow() const;
    };
}