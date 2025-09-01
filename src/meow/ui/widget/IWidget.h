#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../../driver/graphics/GraphicsDriver.h"

namespace meow
{
    class IWidget
    {

    public:
        enum TypeID : uint8_t
        {
            TYPE_ID_IMAGE = 0,
            TYPE_ID_KEYBOARD,
            TYPE_ID_KB_ROW,
            TYPE_ID_EMPTY_LAYOUT,
            TYPE_ID_MENU_ITEM,
            TYPE_ID_TOGGLE_ITEM,
            TYPE_ID_SPIN_ITEM,
            TYPE_ID_DYN_MENU,
            TYPE_ID_FIX_MENU,
            TYPE_ID_NAVBAR,
            TYPE_ID_NOTIFICATION,
            TYPE_ID_PROGRESSBAR,
            TYPE_ID_SCROLLBAR,
            TYPE_ID_SPINBOX,
            TYPE_ID_LABEL,
            TYPE_ID_TEXTBOX,
            TYPE_ID_TOGGLE_SWITCH,
        };

        enum Alignment : uint8_t
        {
            ALIGN_START = 0,
            ALIGN_CENTER,
            ALIGN_END
        };

        enum Gravity : uint8_t
        {
            GRAVITY_TOP = 0,
            GRAVITY_CENTER,
            GRAVITY_BOTTOM
        };

        enum Orientation : uint8_t
        {
            HORIZONTAL = 0,
            VERTICAL,
        };

        enum Visibility : uint8_t
        {
            VISIBLE = 0,
            INVISIBLE,
        };

        IWidget(uint16_t widget_ID, IWidget::TypeID type_ID, bool is_container = false);
        virtual ~IWidget() = 0;

        /**
         * @brief Повертає вказівник на глибоку копію віджета.
         *
         * @param id Ідентифікатор, який буде присвоєно новому віджету.
         * @return IWidget*
         */
        virtual IWidget *clone(uint16_t id) const = 0;

        /**
         * @brief Викликає процедуру малювання віджета на дисплей.
         * Якщо віджет не було змінено, він автоматично пропустить перемальовування.
         *
         */
        virtual void onDraw() = 0;

        /**
         * @brief Повертає ідентифікатор типу віджета.
         *
         * @return TypeID
         */
        TypeID getTypeID() const { return _type_ID; }

        /**
         * @brief Якщо можливо, приводить об'єкт до відповідного типу.
         * Є сенс викликати для *IWidget.
         *
         * @tparam T Тип, до якого потрібно привести віджет.
         * @return T* - Вказівник на віджет, якщо приведення можливе.
         * @return nullptr - Інакше.
         */
        template <typename T>
        T *castTo();

        /**
         * @brief Викликає примусове перемальовування віджета,
         * навіть, якщо його не було змінено.
         *
         */
        void forcedDraw();

        /**
         * @brief Встановлює позицію віджета відносно лівого верхнього кута свого батьківського контейнера.
         * Або відносно лівого верхнього кута дисплею, якщо батьківський контейнер відсутній.
         *
         * @param x Координата.
         * @param y Координата.
         */
        void setPos(uint16_t x, uint16_t y);

        /**
         * @brief Встановлює висоту віджета.
         *
         * @param height
         */
        void setHeight(uint16_t height);

        /**
         * @brief Встановлює ширину віджета.
         *
         * @param width
         */
        void setWidth(uint16_t width);

        /**
         * @brief Встановлює колір фону віджета.
         *
         * @param back_color
         */
        void setBackColor(uint16_t back_color);

        /**
         * @brief Встановлює вказівник на батьківський контейнер.
         *
         * @param parent
         */
        void setParent(IWidget *parent);

        /**
         * @brief Повертає вказівник на батьківський контейнер.
         *
         * @return const IWidget*
         */
        const IWidget *getParent() const { return _parent; }

        /**
         * @brief Встановлює значення скруглення кутів віджета.
         *
         * @param radius Значення не повинне перевищувати половину довжини коротшої сторони віджета.
         */
        void setCornerRadius(uint8_t radius);

        /**
         * @brief Встановлює стан прапору, який керує механізмом відображення межі віджету.
         * Товщина межі становить завжди 1 піксель.
         * Межа буде відображатися за рахунок ширини віджета.
         *
         * @param state Якщо true - межа віджета буде відображатися.
         */
        void setBorder(bool state);

        /**
         * @brief Встановлює колір межі віджета.
         *
         * @param color
         */
        void setBorderColor(uint16_t color);

        /**
         * @brief Повертає X координату віджета відносно верхнього лівого кута дисплею.
         *
         * @return uint16_t
         */
        uint16_t getXPos() const;

        /**
         * @brief  Повертає Y координату віджета відносно верхнього лівого кута дисплею.
         *
         * @return uint16_t
         */
        uint16_t getYPos() const;

        /**
         * @brief Повертає локальну X координату віджета.
         *
         * @return uint16_t
         */
        uint16_t getXPosLoc() const { return _x_pos; }

        /**
         * @brief Повертає локальну Y координату віджета.
         *
         * @return uint16_t
         */
        uint16_t getYPosLoc() const { return _y_pos; }

        /**
         * @brief Повертає значення скруглення кутів віджета.
         *
         * @return uint8_t
         */
        uint8_t getCornerRadius() const { return _corner_radius; }

        /**
         * @brief Повертає ідентифікатор віджета, який йому було присвоєно під час створення.
         *
         * @return uint16_t
         */
        uint16_t getID() const { return _id; }

        /**
         * @brief Повертає висоту віджета у пікселях.
         *
         * @return uint16_t
         */
        uint16_t getHeight() const { return _height; }

        /**
         * @brief Повертає ширину віджета у пікселях.
         *
         * @return uint16_t
         */
        uint16_t getWidth() const { return _width; }

        /**
         * @brief Повертає колір фону віджета.
         *
         * @return uint16_t
         */
        uint16_t getBackColor() const { return _back_color; }

        /**
         * @brief Повертає колір межі віджета.
         *
         * @return uint16_t
         */
        uint16_t getBorderColor() const { return _border_color; }

        /**
         * @brief Повертає стан пропору, який керує маханізмом відображення межі віджета.
         *
         * @return true - Якщо межа віджета відображається.
         * @return false - Інакше.
         */
        bool hasBorder() const { return _has_border; }

        /**
         * @brief Встановлює стан прапору, який керує відображенням межі віджета та її кольором, під час потрапляння фокусу на нього.
         *
         * @param state Якщо true - межа буде відображатися в заданому кольорі під час отримання фокусу віджетом.
         * Попередній стан межі ігнорується, та буде відновлено після видалення фокусу з віджета.
         */
        void setChangingBorder(bool state);

        /**
         * @brief Встановлює стан прапору, який керує зміною кольору фону під час отримання фокусу віджетом.
         *
         * @param state Якщо true - фоновий колір віджета буде змінюватися відповідно до налаштувань.
         * Попереднє значення кольору фону буде автоматично відновлено після видалення фокусу з віджета.
         */
        void setChangingBack(bool state);

        /**
         * @brief Встановлює колір межі віджета у фокусі.
         *
         * @param color
         */
        void setFocusBorderColor(uint16_t color);

        /**
         * @brief Повертає значення кольору межі віджета у фокусі.
         *
         * @return uint16_t
         */
        uint16_t getFocusBorderColor() const { return _focus_border_color; }

        /**
         * @brief Встановлює колір фону віджета у фокусі.
         *
         * @param color
         */
        void setFocusBackColor(uint16_t color);

        /**
         * @brief Повертає колір фону віджета у фокусі.
         *
         * @return uint16_t
         */
        uint16_t getFocusBackColor() const { return _focus_back_color; }

        /*!
         * @brief Переводить віджет до стану "У фокусі".
         */
        void setFocus();

        /*!
         * @brief Видаляє стан "У фокусі" з віджета.
         */
        void removeFocus();

        /**
         * @brief Встановлює видимість віджета. Якщо віджет переведено в невидимий стан, він не видаляється,
         * але пропускає своє малювання або замальовує своє місце розташування фоновим кольором, якщо викликано примусове малювання.
         *
         * @param value Може мати значення: VISIBLE / INVISIBLE
         */
        void setVisibility(Visibility value);

        /**
         * @brief Повертає поточний стан видимості віджета.
         *
         * @return Visibility
         */
        Visibility getVisibility() const { return _visibility; }

        /**
         * @brief Перевіряє чи пересікається віджет з указаними координатами.
         *
         * @param x Координата.
         * @param y Координата.
         * @return true - Якщо віджет пересікається з цими координатами.
         * @return false - Інакше.
         */
        bool hasIntersectWithCoords(uint16_t x, uint16_t y);

        /**
         * @brief Повертає стан прапору, який вказує на те, чи являється цей віджет контейнером для інших віджетів.
         *
         * @return true - Якщо віджет є контейнером.
         * @return false - Інакше.
         */
        bool isContainer() const { return _is_container; }

        /**
         * @brief Встановлює прозорість фону віджета.
         *
         * @param state Якщо true - фон віджета не замальовується фоновим кольором.
         * Для коректного відображення необхідно самостійно перемальовувати зображення під прозорим віджетом.
         */
        void setTransparency(bool state) { _is_transparent = state; }

        /**
         * @brief Повертає стан прапору, який вказує, чи є віджет прозорим.
         *
         * @return true - Якщо віджет є прозорим.
         * @return false - Інакше.
         */
        bool isTransparent() const { return _is_transparent; }

    protected:
        /*!
         * @brief  Залити місце розташування віджета фоновим кольором.
         */
        void clear();

        /*!
         * @brief  Приховати елемент. Працює, якщо віджет має батьківський контейнер.
         */
        void hide();

    protected:
        const IWidget *_parent{nullptr};

        uint16_t _x_pos{0};
        uint16_t _y_pos{0};
        uint16_t _width{1};
        uint16_t _height{1};
        uint16_t _back_color{0x0000};
        uint16_t _border_color{0x0000};
        uint16_t _focus_border_color{0xFFFF};
        uint16_t _old_border_color{0xFFFF};
        uint16_t _focus_back_color{0xFFFF};
        uint16_t _old_back_color{0xFFFF};

    private:
        const uint16_t _id;

    protected:
        Visibility _visibility{VISIBLE};
        uint8_t _corner_radius{0};

        bool _is_changed{true};
        bool _has_border{false};
        bool _is_transparent{false};
        bool _has_focus{false};
        bool _old_border_state{false};
        bool _need_clear_border{false};
        bool _need_change_border{false};
        bool _need_change_back{false};

    private:
        const TypeID _type_ID;
        const bool _is_container;
    };

    /**
     * @brief Якщо можливо, приводить об'єкт до відповідного типу.
     * Є сенс викликати для *IWidget.
     *
     * @tparam T Тип, до якого потрібно привести віджет.
     * @return T* - Вказівник на віджет, якщо приведення можливе.
     * @return nullptr - Інакше.
     */
    template <typename T>
    inline T *IWidget::castTo()
    {
        if (_type_ID == T::staticType())
            return static_cast<T *>(this);
        else
        {
            log_e("Некоректне приведення типу %u до %u", T::staticType(), _type_ID);
            return nullptr;
        }
    }
}