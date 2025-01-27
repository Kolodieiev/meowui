#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>

#include "../../../widget/image/Image.h"
#include "../../../widget/text/Label.h"

namespace meow
{
    class MenuItem : public IWidget
    {
    public:
        MenuItem(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~MenuItem();

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
         * @return MenuItem*
         */
        MenuItem *clone(uint16_t id) const override;

        /**
         * @brief Встановлює вказівник на віджет Image, що буде використано в якості іконки.
         * Image не буде видалено автоматично разом з віджетом.
         * Один і той же Image може бути закріплений за різними елементами списку.
         *
         * @param img Вказівник на Image.
         */
        void setIco(Image *img_ptr);

        /**
         * @brief Встановлює вказівник на віджет Label, що буде використаний для відображення тексту елемента списку.
         * Label буде видалено автоматично разом з віджетом.
         * Для кожного елемента списку повинен використовуватися власний Label.
         *
         * @param lbl Вказівник на Label.
         */
        void setLbl(Label *lbl_ptr);

        /**
         * @brief Повертає вказівник на Label, який присвоєно цьому елементу списку.
         *
         * @return Label* - Вказівник на віджет.
         * @return nullptr - Якщо вказівник не було встановлено раніше.
         */
        Label *getLabel() const { return _label; }

        /**
         * @brief Повертає вказівник на Image, який присвоєно цьому елементу списку.
         *
         * @return Image* - Вказівник на віджет.
         * @return nullptr - Якщо вказівник не було встановлено раніше.
         */
        Image *getIco() const { return _ico; }

        /**
         * @brief Повертає копію тексту, що міститься у цьому елементі списку.
         * Викликає перезавантаження МК, якщо цьому елементу раніше не було присвоєно текстову мітку.
         *
         * @return String
         */
        String getText() const;

    protected:
        const uint8_t ITEM_PADDING{5};
        using IWidget::setVisibility;

        Image *_ico{nullptr};
        Label *_label{nullptr};
    };
}