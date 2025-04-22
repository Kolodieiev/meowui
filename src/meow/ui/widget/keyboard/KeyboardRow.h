#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../IWidgetContainer.h"

namespace meow
{
    class KeyboardRow final : public IWidgetContainer
    {

    public:
        KeyboardRow(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~KeyboardRow() {}

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
         * @return KeyboardRow*
         */
        virtual KeyboardRow *clone(uint16_t id) const override;

        /**
         * @brief Повертає ідентифікатор типу.
         * Використовується в системі приведення типу.
         *
         * @return constexpr TypeID
         */
        static constexpr TypeID staticType() { return TypeID::TYPE_ID_KB_ROW; }

        /**
         * @brief Повертає ідентифікатор віджета, на якому встановлено фокус.
         *
         * @return uint16_t
         */
        uint16_t getCurrentBtnID() const;

        /**
         * @brief Повертає копію тексту, що міститься у віджеті, на якому встановлено фокус.
         *
         * @return String
         */
        String getCurrentBtnTxt() const;

        /**
         * @brief Переміщує фокус у контейнері на один віджет вгору.
         *
         * @return true - Якщо операція виконана успішно.
         * @return false - Інакше.
         */
        bool focusUp();

        /**
         * @brief Переміщує фокус у контейнері на один віджет униз.
         *
         * @return true - Якщо операція виконана успішно.
         * @return false - Інакше.
         */
        bool focusDown();

        /**
         * @brief Встановлює висоту кнопок у цьому контейнері.
         *
         * @param height Значення висоти кнопок у пікселях.
         */
        void setBtnHeight(uint16_t height)
        {
            _btn_height = height > 0 ? height : 1;
            _is_changed = true;
        }

        /**
         * @brief Повертає значення висоти кнопок у пікселях.
         *
         * @return uint16_t
         */
        uint16_t getBtnsHeight() const { return _btn_height; }

        /**
         * @brief Встановлює ширину кнопок у цьому контейнері.
         *
         * @param width Значення ширини кнопок у пікселях.
         */
        void setBtnWidth(uint16_t width)
        {
            _btn_width = width > 0 ? width : 1;
            _is_changed = true;
        }

        /**
         * @brief Повертає значення ширини кнопок у пікселях.
         *
         * @return uint16_t
         */
        uint16_t getBtnsWidth() const { return _btn_width; }

        /**
         * @brief Повертає позицію кнопки для данного ряду, на якій встановлено фокус.
         *
         * @return uint16_t
         */
        uint16_t getCurFocusPos() const { return _cur_focus_pos; }

        /**
         * @brief Встановлює фокус на віджеті з указаним порядковим номером.
         *
         * @param pos Порядковий номер віджета кнопки у контейнері.
         */
        void setFocus(uint16_t pos);

        /**
         * @brief Видаляє фокус з віджета в даному контейнері.
         *
         */
        void removeFocus();

    private:
        uint16_t _cur_focus_pos{0};

        uint16_t _btn_height{1};
        uint16_t _btn_width{1};

        IWidget *getFocusBtn() const;
    };
}