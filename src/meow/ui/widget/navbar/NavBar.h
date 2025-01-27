#pragma once
#include <Arduino.h>
#include "../IWidget.h"

namespace meow
{

    class NavBar : public IWidget
    {
    public:
        NavBar(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~NavBar();

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
         * @return NavBar* 
         */
        NavBar *clone(uint16_t id) const override;

        /**
         * @brief Встановлює віджети, які будуть відображатися на панелі навігації.
         * 
         * @param start Крайній лівий віджет.
         * @param middle Центральний віджет.
         * @param last Крайній правий віджет.
         */
        void setWidgets(IWidget *start, IWidget *middle, IWidget *last);

    private:
        using IWidget::isTransparent;
        using IWidget::setTransparency;
        IWidget *_first{nullptr};
        IWidget *_middle{nullptr};
        IWidget *_last{nullptr};
    };

}