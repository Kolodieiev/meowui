#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "Menu.h"

namespace meow
{
    typedef std::function<void(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg)> OnNextItemsLoad;
    typedef std::function<void(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg)> OnPrevItemsLoad;

    class DynamicMenu final : public Menu
    {
    public:
        DynamicMenu(uint16_t widget_ID);
        virtual ~DynamicMenu() {}

        /**
         * @brief Повертає вказівник на глибоку копію віджета.
         *
         * @param id Ідентифікатор, який буде присвоєно новому віджету.
         * @return DynamicMenu*
         */
        virtual DynamicMenu *clone(uint16_t id) const override;

        /**
         * @brief Повертає ідентифікатор типу.
         * Використовується в системі приведення типу.
         *
         * @return constexpr TypeID
         */
        static constexpr TypeID staticType() { return TypeID::TYPE_ID_DYN_MENU; }

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
         * @brief Встановлює обробник, який буде викликано для завантаження наступної частини віджетів для меню.
         *
         * @param handler Обробник.
         * @param arg Аргумент, що буде передано в обробник.
         */
        void setOnNextItemsLoadHandler(OnNextItemsLoad handler, void *arg)
        {
            _next_items_load_handler = handler;
            _next_items_load_arg = arg;
        }

        /**
         * @brief Встановлює обробник, який буде викликано для завантаження попередньої частини віджетів для меню.
         *
         * @param handler Обробник.
         * @param arg Аргумент, що буде передано в обробник.
         */
        void setOnPrevItemsLoadHandler(OnPrevItemsLoad handler, void *arg)
        {
            _prev_items_load_handler = handler;
            _prev_items_load_arg = arg;
        }

    private:
        OnNextItemsLoad _next_items_load_handler{nullptr};
        void *_next_items_load_arg{nullptr};
        //
        OnPrevItemsLoad _prev_items_load_handler{nullptr};
        void *_prev_items_load_arg{nullptr};
    };

}