#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../IWidgetContainer.h"
#include "./item/MenuItem.h"

namespace meow
{
    class Menu : public IWidgetContainer
    {

    public:
        Menu(uint16_t widget_ID, IWidget::TypeID type_ID);
        virtual ~Menu() {}

        virtual bool focusUp() = 0;
        virtual bool focusDown() = 0;

        /**
         * @brief Викликає процедуру малювання віджета на дисплей.
         * Якщо віджет не було змінено, він автоматично пропустить перемальовування.
         *
         */
        virtual void onDraw() override;

        /**
         * @brief Видаляє всі елементи списку меню.
         *
         */
        void delWidgets();

        /**
         * @brief Видаляє елемент списку з меню і пам'яті за його ідентифікатором.
         *
         * @param widget_ID Ідентифікатор елемента списку.
         * @return true - у разі успіху операції.
         * @return false - інакше.
         */
        bool delWidgetByID(uint16_t widget_ID);

        /**
         * @brief Встановлює висоту кожного із елементів списку для вертикальної орієнтації меню.
         *
         * @param height Висота кожного елемента списку.
         */
        void setItemHeight(uint16_t height)
        {
            _item_height = height > 0 ? height : 1;
            _is_changed = true;
        }

        /**
         * @brief Повертає висоту кожного елемента списку.
         *
         * @return uint16_t
         */
        uint16_t getItemHeight() const { return _orientation == VERTICAL ? _item_height : _height - 4; }

        /**
         * @brief Встановлює ширину кожного із елементів списку для горизонтальної орієнтації меню.
         *
         * @param width
         */
        void setItemWidth(uint16_t width)
        {
            _item_width = width > 0 ? width : 1;
            _is_changed = true;
        }

        /**
         * @brief Повертає ширину кожного елемента списку.
         *
         * @return uint16_t
         */
        uint16_t getItemWidth() const { return _orientation == HORIZONTAL ? _item_width : _width - 4; }

        /**
         * @brief Встановлює орієнтацію меню.
         * При вертикальній орієнтації елементи меню розташовуються зверху вниз.
         * При горизонтальній - зліва на право.
         *
         * @param orientation Може мати значення: VERTICAL / HORIZONTAL.
         */
        void setOrientation(const Orientation orientation)
        {
            _orientation = orientation;
            _is_changed = true;
        }

        /**
         * @brief Повертає поточну орієнтацію меню.
         *
         * @return Orientation
         */
        Orientation getOrientation() const { return _orientation; }

        /**
         * @brief Повертає ідентифікатор віджета елемента списку, на якому встановлено фокус.
         *
         * @return uint16_t - Ідентифікатор віджета. 0 - Якщо контейнер порожній.
         */
        uint16_t getCurrItemID() const;

        /**
         * @brief Повертає порядковий номер віжета, на якому встановлено фокус, в контейнері.
         *
         * @return uint16_t - Позиція віджета в контейнері. 0 - Якщо контейнер порожній.
         */
        uint16_t getCurrFocusPos() const { return _cur_focus_pos; };

        /**
         * @brief Повертає копію тексту, що міститься у віджеті елемента списку, на якому встановлено фокус.
         *
         * @return String
         */
        String getCurrItemText() const;

        /**
         * @brief Повертає вказівник на віджет елемента списку, на якому встановлено фокус.
         *
         * @return IWidget* - Вказівник на віджет елемента списку.
         * @return nullptr - Якщо контейнер порожній.
         */
        IWidget *getCurrItem();

        /**
         * @brief Встановлює відступи між елементами меню.
         *
         * @param items_spacing Розмір відступу у пікселях між елементами списку.
         */
        void setItemsSpacing(uint16_t items_spacing) { _items_spacing = items_spacing; }

        /**
         * @brief Повертає розмір відступу між елементами списку.
         *
         * @return uint16_t
         */
        uint16_t getItemsSpacing() const { return _items_spacing; }

        /**
         * @brief Додає вказівник на віджет елемента списку до контейнера меню.
         * Віджет повинен мати уникальний ідентифікатор для цього контейнера.
         * Ідентифікатор віджета повинен бути більшим за 0.
         *
         * @param item Вказівник на віджет елемента списку.
         */
        void addItem(MenuItem *item_ptr);

        /**
         * @brief Розраховує та повертає кількість елементів, що може відобразити меню одночасно за поточних розмір.
         *
         * @return uint16_t
         */
        uint16_t getItemsNumOnScreen() const;

    protected:
        void drawItems(uint16_t start, uint16_t count);
        uint16_t getCyclesCount() const;

        using IWidgetContainer::addWidget;

    private:
        using IWidgetContainer::delWidgetByID;
        using IWidgetContainer::delWidgets;

    protected:
        uint16_t _first_item_index{0};
        uint16_t _cur_focus_pos{0};
        uint16_t _item_height{2};
        uint16_t _item_width{2};
        uint16_t _items_spacing{0};

        Orientation _orientation{VERTICAL};
    };
}