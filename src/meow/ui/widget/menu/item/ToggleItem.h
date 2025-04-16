#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>

#include "./MenuItem.h"
#include "../../../widget/toggle/ToggleSwitch.h"

namespace meow
{
    class ToggleItem final : public MenuItem
    {
    public:
        ToggleItem(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~ToggleItem();

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
         * @return ToggleItem*
         */
        ToggleItem *clone(uint16_t id) const override;

        /**
         * @brief Встановлює вказівник на віджет ToggleSwitch, що буде відображатися віджеті елемента списку.
         * ToggleSwitch буде видалено автоматично разом з віджетом.
         * Для кожного елемента списку повинен використовуватися власний ToggleSwitch.
         *
         * @param togg_switch_ptr Вказівник на ToggleSwitch.
         */
        void setToggle(ToggleSwitch *togg_switch_ptr);

        /**
         * @brief Встановлює стан ToggleSwitch у "Ввімкнений".
         *
         */
        void on();

        /**
         * @brief Встановлює стан ToggleSwitch у "Вимкнений".
         *
         */
        void off();

        /**
         * @brief Повертає значення прапору, що вказує на поточний стан ToggleSwitch.
         *
         * @return true - Якщо ToggleSwitch знаходиться у стані "Ввімкнений".
         * @return false - Якщо ToggleSwitch знаходиться у стані "Вимкнений"
         * або вказівник на ToggleSwitch не було встановлено..
         */
        bool isOn() const;

    private:
        using IWidget::setVisibility;

        ToggleSwitch *_toggle{nullptr};
    };
}