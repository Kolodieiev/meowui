/*
 * Генератор віджетів.
 * Використовується тут для часткового очищення файлів вікон від однотипного коду.
 */

#pragma once
#include <Arduino.h>
#include "meow/driver/graphics/GraphicsDriver.h"

#include "meow/ui/widget/layout/EmptyLayout.h"
#include "meow/ui/widget/text/Label.h"
#include "meow/ui/widget/image/Image.h"
#include "meow/ui/widget/menu/item/MenuItem.h"
#include "meow/ui/widget/menu/DynamicMenu.h"
#include "meow/ui/widget/keyboard/Keyboard.h"

using namespace meow;

class WidgetCreator
{
public:
    WidgetCreator(GraphicsDriver &display);

    EmptyLayout *getEmptyLayout();
    Label *getDisplayDescription(uint16_t id, const char *text);
    Label *getItemLabel(const char *text, uint8_t font_id = 2, uint8_t text_size = 1);
    MenuItem *getMenuItem(uint16_t id = 1);
    DynamicMenu *getDynamicMenu(uint16_t id);
    Label *getStatusMsgLable(uint16_t id, const char *text, uint8_t text_size = 1);
    Keyboard *getStandardEnKeyboard(uint16_t id);

private:
    GraphicsDriver &_display;
};
