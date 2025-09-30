#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include "./MenuItem.h"
#include "meow/ui/widget/spinbox/SpinBox.h"

namespace meow
{
    // cppcheck-suppress duplInheritedMember
    class SpinItem : public MenuItem
    {
    public:
        explicit SpinItem(uint16_t widget_ID);
        virtual ~SpinItem() override;

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
         * @return SpinItem*
         */
        SpinItem *clone(uint16_t id) const override;

        /**
         * @brief Повертає ідентифікатор типу.
         * Використовується в системі приведення типу.
         *
         * @return constexpr TypeID
         */
        static constexpr TypeID staticType() { return TypeID::TYPE_ID_SPIN_ITEM; }

        /**
         * @brief Встановлює вказівник на віджет SpinBox, що буде відображатися у віджеті елемента списку.
         * SpinBox буде видалено автоматично разом з віджетом.
         * Для кожного елемента списку повинен використовуватися власний SpinBox.
         *
         * @param spinbox_ptr Вказівник на SpinBox.
         */
        void setSpin(SpinBox *spinbox_ptr);

        /**
         * @brief Повертає вказівник на SpinBox, який присвоєно цьому елементу списку.
         *
         * @return SpinBox* - Вказівник на віджет.
         */
        SpinBox *getSpin() const { return _spinbox; }

        /**
         * @brief Перевикликає відповідний метод у об'єкта SpinBox, що міститься у цьому елементі списку.
         *
         */
        void up();

        /**
         * @brief Перевикликає відповідний метод у об'єкта SpinBox, що міститься у цьому елементі списку.
         *
         */
        void down();

        /**
         * @brief Перевикликає відповідний метод у об'єкта SpinBox, що міститься у цьому елементі списку.
         *
         * @param min
         */
        void setMin(float min);

        /**
         * @brief Перевикликає відповідний метод у об'єкта SpinBox, що міститься у цьому елементі списку.
         *
         * @param max
         */
        void setMax(float max);

        /**
         * @brief Перевикликає відповідний метод у об'єкта SpinBox, що міститься у цьому елементі списку.
         *
         * @param value
         */
        void setValue(float value);

        /**
         * @brief Перевикликає відповідний метод у об'єкта SpinBox, що міститься у цьому елементі списку.
         *
         * @return float
         */
        float getValue() const;

    private:
        using IWidget::setVisibility;

    private:
        SpinBox *_spinbox{nullptr};
    };
}