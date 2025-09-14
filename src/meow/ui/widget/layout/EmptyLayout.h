#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../IWidgetContainer.h"

namespace meow
{
  class EmptyLayout : public IWidgetContainer
  {
  public:
    explicit EmptyLayout(uint16_t widget_ID, IWidget::TypeID type_ID = TYPE_ID_EMPTY_LAYOUT);
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

    /**
     * @brief Повертає ідентифікатор типу.
     * Використовується в системі приведення типу.
     *
     * @return constexpr TypeID
     */
    static constexpr TypeID staticType() { return TypeID::TYPE_ID_EMPTY_LAYOUT; }
  };
}