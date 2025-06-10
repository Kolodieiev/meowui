#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../IWidget.h"

namespace meow
{
    class ScrollBar final : public IWidget
    {
    public:
        explicit ScrollBar(uint16_t widget_ID);
        virtual ~ScrollBar() {}

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
         * @return ScrollBar*
         */
        virtual ScrollBar *clone(uint16_t id) const override;

        /**
         * @brief Повертає ідентифікатор типу.
         * Використовується в системі приведення типу.
         *
         * @return constexpr TypeID
         */
        static constexpr TypeID staticType() { return TypeID::TYPE_ID_SCROLLBAR; }

        /**
         * @brief Переміщує слайдер на одну позицію вгору, якщо можливо.
         *
         * @return true - У разі успіху операції.
         * @return false - Інакше.
         */
        bool scrollUp();

        /**
         * @brief Переміщує слайдер на одну позицію вниз, якщо можливо.
         *
         * @return true - У разі успіху операції.
         * @return false - Інакше.
         */
        bool scrollDown();

        /**
         * @brief Викликати тільки після ініціалізації ширини і висоти смуги прокрутки.
         * Встановлює кількість позицій смуги прокрутки.
         *
         * @param max_value Кількість позицій смуги прокрутки.
         */
        void setMax(uint16_t max_value);

        /**
         * @brief Встановлює нову позицію слайдера.
         *
         * @param value Нова позицію слайдера.
         */
        void setValue(uint16_t value);

        /**
         * @brief Встановлює орієнтацію смуги прокрутки.
         * По замовченню встановлено VERTICAL.
         *
         * @param orientation Може мати значення: VERTICAL / HORIZONTAL.
         */
        void setOrientation(Orientation orientation)
        {
            _orientation = orientation;
            _is_changed = true;
        }

        /**
         * @brief Повертає поточну орієнтацію смуги прокрутки.
         *
         * @return Orientation
         */
        Orientation getOrientation() const { return _orientation; }

        /**
         * @brief Встановлює колір слайдера.
         *
         * @param color Новий колір слайдера.
         */
        void setSliderColor(uint16_t color)
        {
            _slider_color = color;
            _is_changed = true;
        }

        /**
         * @brief Повертає поточну позицію слайдера.
         *
         * @return uint16_t
         */
        uint16_t getValue() const { return _cur_value; }

        /**
         * @brief Повертає максимально можливе значення позиції.
         *
         * @return uint16_t
         */
        uint16_t getMax() const { return _max_value; }

    private:
        using IWidget::isTransparent;
        using IWidget::setTransparency;

    private:
        uint16_t _max_value{1};
        uint16_t _cur_value{0};
        uint16_t _slider_last_x_pos{0};
        uint16_t _slider_last_y_pos{0};
        uint16_t _slider_color{0xFFFF};
        uint16_t _smart_value{0};

        Orientation _orientation{VERTICAL};

        uint8_t _slider_width{1};
        uint8_t _slider_height{1};
        uint8_t _slider_step_size{1};
        uint8_t _steps_to_scroll{0};
        uint8_t _steps_counter{0};
        uint8_t _scroll_direction{0};

        bool _smart_scroll_enabled{false};
    };

}