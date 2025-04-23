#pragma once
#include <Arduino.h>
#include "../IWidget.h"

namespace meow
{
    class ToggleSwitch final : public IWidget
    {
    public:
        ToggleSwitch(uint16_t widget_ID) : IWidget(widget_ID, TYPE_ID_TOGGLE_SWITCH) {}
        virtual ~ToggleSwitch() {};

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
         * @return ToggleSwitch*
         */
        virtual ToggleSwitch *clone(uint16_t id) const override;

        /**
         * @brief Повертає ідентифікатор типу.
         * Використовується в системі приведення типу.
         *
         * @return constexpr TypeID
         */
        static constexpr TypeID staticType() { return TypeID::TYPE_ID_TOGGLE_SWITCH; }

        /**
         * @brief  Повертає поточний стан перемикача.
         *
         * @return true - Якщо перемикач знаходиться в положенні "Увімкнутий".
         * @return false - Якщо перемикач знаходиться в положонні "Вимкнутий".
         */
        bool isOn() const { return _is_on; }

        /**
         * @brief Встановлює стан перемикача у "Ввімкнений".
         *
         */
        void on()
        {
            _is_on = true;
            _is_changed = true;
        }

        /**
         * @brief Встановлює стан перемикача у "Вимкнутий".
         *
         */
        void off()
        {
            _is_on = false;
            _is_changed = true;
        }

        /**
         * @brief Встановлює колір важелю.
         *
         * @param color
         */
        void setLeverColor(uint16_t color)
        {
            _lever_color = color;
            _is_changed = true;
        }

        /**
         * @brief Повертає значення кольору важелю.
         *
         * @return uint16_t
         */
        uint16_t getLeverColor() const { return _lever_color; }

        /**
         * @brief Встановлює колір фону перемикача у стані "Увімкнений".
         *
         * @param color
         */
        void setOnColor(uint16_t color)
        {
            _on_color = color;
            _is_changed = true;
        }

        /**
         * @brief Повертає значення кольору фону перемикача у стані "Увімкнений".
         *
         * @return uint16_t
         */
        uint16_t getOnColor() const { return _on_color; }

        /**
         * @brief Встановлює колір фону перемикача у стані "Ввимкнений".
         *
         * @param color
         */
        void setOffColor(uint16_t color)
        {
            _off_color = color;
            _is_changed = true;
        }

        /**
         * @brief Повертає значення кольору фону перемикача у стані "Вимкнений".
         *
         * @return uint16_t
         */
        uint16_t getOffColor() const { return _off_color; }

        /**
         * @brief Встановлює орієнтацію відображення перемикача.
         *
         * @param orientation Може мати значення: ORIENTATION_HORIZONTAL / ORIENTATION_VERTICAL.
         */
        void setOrientation(Orientation orientation)
        {
            _orientation = orientation;
            _is_changed = true;
        }

        /**
         * @brief Повертає значення орієнтації відображення перемикача.
         *
         * @return Orientation
         */
        Orientation getOrientation() const { return _orientation; }

    private:
        using IWidget::getBackColor;
        using IWidget::isTransparent;
        using IWidget::setBackColor;
        using IWidget::setTransparency;

        Orientation _orientation{ORIENTATION_HORIZONTAL};
        bool _is_on{false};
        uint16_t _lever_color{0xFFFF};
        uint16_t _on_color{TFT_GREEN};
        uint16_t _off_color{TFT_RED};
    };
}