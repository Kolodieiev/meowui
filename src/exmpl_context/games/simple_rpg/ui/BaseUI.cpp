#include "BaseUI.h"
const char STR_BUNN_COUNTER[] = "Лічильник кроликів: ";

namespace simple_rpg
{
    BaseUI::BaseUI() 
    {
        // Створюємо та налаштовуємо Label
        _bunny_counter_lbl = new Label(1);
        _bunny_counter_lbl->setText(STR_BUNN_COUNTER);
        _bunny_counter_lbl->setPos(0, _display.height() - _bunny_counter_lbl->getHeight());
        _bunny_counter_lbl->setWidth(_display.width());
        _bunny_counter_lbl->setAlign(IWidget::ALIGN_CENTER);
        _bunny_counter_lbl->setGravity(IWidget::GRAVITY_CENTER);
        _bunny_counter_lbl->setBackColor(TFT_BLACK);
        _bunny_counter_lbl->setTextColor(TFT_RED);
    }

    void BaseUI::onDraw()
    {
        // Відмальовуємо лічильник кроликів через примусове малювання.
        // Через звичайне малювання, оптимізатор не дасть відмалюватися Label-у.
        _bunny_counter_lbl->forcedDraw();
    }

    void BaseUI::updBunnyNum(uint16_t value)
    {
        // Формуємо рядок, та встановлюємо його в Label
        String lbl = STR_BUNN_COUNTER;
        lbl += String(value);
        _bunny_counter_lbl->setText(lbl);
    }
}