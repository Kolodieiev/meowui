#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>

#include "meow/ui/widget/image/Image.h"
#include "meow/ui/widget/text/Label.h"

namespace meow
{
    class MenuItem : public IWidget
    {
    public:
        MenuItem(uint16_t widget_ID, IWidget::TypeID type_ID = TYPE_ID_UNKNOWN);
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
         * @brief Повертає ідентифікатор типу.
         * Використовується в системі приведення типу.
         *
         * @return constexpr TypeID
         */
        static constexpr TypeID staticType() { return TypeID::TYPE_ID_MENU_ITEM; }

        /**
         * @brief Встановлює вказівник на віджет Image, що буде використано в якості іконки.
         * Image не буде видалено автоматично разом з віджетом.
         * Один і той же Image може бути закріплений за різними елементами списку.
         *
         * @param img Вказівник на Image.
         */
        void setImg(Image *img_ptr);

        /**
         * @brief Повертає вказівник на Image, який присвоєно цьому елементу списку.
         *
         * @return Image* - Вказівник на віджет.
         * @return nullptr - Якщо вказівник не було встановлено раніше.
         */
        Image *getImg() const { return _img; }

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
        Label *getLbl() const { return _label; }

        /**
         * @brief Встановлює текст, що буде відображатися у віджеті.
         *
         * @param text
         */
        void setText(const String &text);

        /**
         * @brief Повертає копію тексту, що міститься в цьому елементі списку.
         * Викликає перезавантаження МК, якщо цьому елементу раніше не було присвоєно текстову мітку.
         *
         * @return String
         */
        String getText() const;

    protected:
        const uint8_t ITEM_PADDING{5};
        using IWidget::setVisibility;

        Image *_img{nullptr};
        Label *_label{nullptr};
    };
}