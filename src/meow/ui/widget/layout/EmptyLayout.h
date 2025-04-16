#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../IWidgetContainer.h"

namespace meow
{
  class EmptyLayout : public IWidgetContainer
  {
  public:
    EmptyLayout(uint16_t widget_ID, GraphicsDriver &display, IWidget::ClassID class_ID = CLASS_ID_UNKNOWN);
    virtual ~EmptyLayout();

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
     * @return EmptyLayout*
     */
    EmptyLayout *clone(uint16_t id) const override;
  };
}