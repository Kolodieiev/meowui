#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include "./MenuItem.h"

namespace meow
{
    class ComboItem : public MenuItem
    {
    public:
        explicit ComboItem(uint16_t widget_ID);
        virtual ~ComboItem();

        /**
         * @brief Повертає вказівник на глибоку копію віджета.
         *
         * @param id Ідентифікатор, який буде присвоєно новому віджету.
         * @return SpinItem*
         */
        ComboItem *clone(uint16_t id) const override;

        /**
         * @brief Повертає ідентифікатор типу.
         * Використовується в системі приведення типу.
         *
         * @return constexpr TypeID
         */
        static constexpr TypeID staticType() { return TypeID::TYPE_ID_COMBO_ITEM; }

        /**
         * @brief Додає список пунктів для вибору.
         *
         * @param range
         */
        void setRange(const std::vector<String> &range);

        /**
         * @brief Встановлює активним окремий пункт списку за його порядковим номером.
         *
         * @param pos
         */
        void selectPos(uint16_t pos);

        /**
         * @brief Повертає порядковий номер поточного пункту списку.
         *
         * @return uint16_t
         */
        uint16_t getPos() const { return _curr_pos; }

        /**
         * @brief Змінює поточний пункт списку на одну позицію вгору.
         * Якщо поточна позиція дорівнює 0, тоді вибір переміщуєтсья до останнього пункту списку.
         *
         */
        void scrollUp();

        /**
         * @brief Змінює поточний пункт списку на одну позицію вниз.
         * Якщо поточна позиція дорівнює кількості пунктів, тоді вибір переміщуєтсья до нульового пункту списку.
         *
         */
        void scrollDown();

    private:
        std::vector<String> _range;
        uint16_t _curr_pos{0};
    };
}