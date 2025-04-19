#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../IWidget.h"
#include "Label.h"

namespace meow
{
    // TODO add multiline

    class TextBox : public Label
    {

    public:
        enum FieldType : uint8_t
        {
            TYPE_TEXT = 0,
            TYPE_PASSWORD
        };

        TextBox(uint16_t widget_ID, GraphicsDriver &displa, IWidget::TypeID class_ID = TYPE_ID_UNKNOWN);
        virtual ~TextBox() {}

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
         * @return TextBox*
         */
        virtual TextBox *clone(uint16_t id) const override;

        /**
         * @brief Встановлює тип текстового поля.
         *
         * @param type Може мати значення:
         * TYPE_TEXT - текст відображається регулярними символами.
         * TYPE_PASSWORD - усі символи замінюються на *.
         */
        void setType(FieldType type) { _type = type; }

        /**
         * @brief Повертає тип текстового поля.
         *
         * @return FieldType
         */
        FieldType getType() const { return _type; }

        /**
         * @brief Видаляє останній символ з рядка, що знаходиться в текстовому полі.
         *
         * @return true
         * @return false
         */
        bool removeLastChar();

        /**
         * @brief Додає послідовність символів в кінець рядка текстового поля.
         *
         * @param ch
         */
        void addChars(const char *ch);

    private:
        using Label::getTickerDelay;
        using Label::initWidthToFit;
        using Label::isTicker;
        using Label::isTickerInFocus;
        using Label::setGravity;
        using Label::setTicker;
        using Label::setTickerDelay;
        using Label::setTickerInFocus;
        using Label::updateWidthToFit;

        FieldType _type = TYPE_TEXT;

        uint16_t getFitStr(String &ret_str) const;
    };

}