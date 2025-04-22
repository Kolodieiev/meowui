#pragma once
#include <Arduino.h>
#include "../text/Label.h"

namespace meow
{
    class SpinBox final : public Label
    {

    public:
        enum SpinType : uint8_t
        {
            TYPE_INT = 0,
            TYPE_FLOAT
        };

        SpinBox(uint16_t widget_id, GraphicsDriver &display);
        virtual ~SpinBox() {}

        /**
         * @brief Повертає вказівник на глибоку копію віджета.
         *
         * @param id Ідентифікатор, який буде присвоєно новому віджету.
         * @return SpinBox*
         */
        virtual SpinBox *clone(uint16_t id) const override;

        /**
         * @brief Повертає ідентифікатор типу.
         * Використовується в системі приведення типу.
         *
         * @return constexpr TypeID
         */
        static constexpr TypeID staticType() { return TypeID::TYPE_ID_SPINBOX; }

        /**
         * @brief Збільшує значення, що зберігається у віджеті на 1, якщо можливо.
         * Якщо значення вже досягло заданого максимуму, відбувається перехід на мінімальне.
         *
         */
        void up();

        /**
         * @brief Зменшує значення, що зберігається у віджеті на 1, якщо можливо.
         * Якщо значення вже досягло заданого мінімуму, відбувається перехід на максимальне.
         *
         */
        void down();

        /**
         * @brief Встановлює мінімальне значення, яке може бути встановлено у віджеті.
         *
         * @param min Мінімально допустиме значення.
         */
        void setMin(float min);

        /**
         * @brief Повертає мінімально допустиме значення, яке може бути встановлено у віджеті.
         *
         * @return float
         */
        float getMin() const { return _min; }

        /**
         * @brief Встановлює максимальне значення, яке може бути встановлено у віджеті.
         *
         * @param max Максимально допустиме значення.
         */
        void setMax(float max);

        /**
         * @brief Повертає максимально допустиме значення, яке може бути встановлено у віджеті.
         *
         * @return float
         */
        float getMax() const { return _max; }

        /**
         * @brief Встановлює поточне значення.
         *
         * @param value
         */
        void setValue(float value);

        /**
         * @brief Повертає поточне значення.
         *
         * @return float
         */
        float getValue() const { return _value; }

        /**
         * @brief Встановлює тип відображення вмісту.
         * По замовченню встановлено TYPE_INT.
         * Тобто число відображається без коми.
         *
         * @param spin_type Може мати значення: TYPE_INT / TYPE_FLOAT.
         */
        void setType(SpinType spin_type);

        /**
         * @brief Повертає тип відображення вмісту.
         *
         * @return SpinType
         */
        SpinType getType() const { return _spin_type; }

    private:
        using Label::initWidthToFit;
        using Label::isTicker;
        using Label::isTickerInFocus;
        using Label::setAlign;
        using Label::setGravity;
        using Label::setTicker;
        using Label::setTickerInFocus;
        using Label::updateWidthToFit;

        float _min = 0.0f;
        float _max = 0.0f;
        float _value = 0.0f;

        SpinType _spin_type = TYPE_INT;

        void setSpinValToDraw();
    };
}