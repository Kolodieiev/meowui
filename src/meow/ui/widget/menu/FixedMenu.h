#pragma once
#include <Arduino.h>
#include "Menu.h"

namespace meow
{
    class FixedMenu final : public Menu
    {
    public:
        explicit FixedMenu(uint16_t widget_ID);
        virtual ~FixedMenu() {}

        /**
         * @brief Повертає вказівник на глибоку копію віджета.
         *
         * @param id Ідентифікатор, який буде присвоєно новому віджету.
         * @return FixedMenu*
         */
        virtual FixedMenu *clone(uint16_t id) const override;

        /**
         * @brief Повертає ідентифікатор типу.
         * Використовується в системі приведення типу.
         *
         * @return constexpr TypeID
         */
        static constexpr TypeID staticType() { return TypeID::TYPE_ID_FIX_MENU; }

        /**
         * @brief Переміщує фокус на попередній віджет у контейнері.
         *
         * @return true - Якщо операцію виконано успішно.
         * @return false - Інакше.
         */
        virtual bool focusUp() override;

        /**
         * @brief Переміщує фокус на наступний віджет у контейнері.
         *
         * @return true - Якщо операцію виконано успішно.
         * @return false - Інакше.
         */
        virtual bool focusDown() override;

        /**
         * @brief Встановлює стан прапору, який відповідає за механізм зациклювання меню.
         *
         * @param state Якщо true - меню буде зациклене.
         * Якщо false - фокус не буде переходити на протилежний край списку при досягненні його кінця.
         */
        void setLoopState(bool state) { _is_loop_enbl = state; }

        /**
         * @brief Встановлює фокус в списку на віджеті за його порядковим номером у контейнері.
         * Якщо елемент на вказаній позиції відсутній, поточний фокус не буде змінено.
         *
         * @param focus_pos Порядковий номер віджета в контейнері.
         */
        void setCurrFocusPos(uint16_t focus_pos);

    protected:
        bool _is_loop_enbl = false;
    };
}