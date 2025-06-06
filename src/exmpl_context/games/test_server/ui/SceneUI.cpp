#include "SceneUI.h"

const char STR_WAIT_CLIENTS[] = "Очікуємо клієнтів";

namespace test_server
{
    SceneUI::SceneUI() 
    {
    }

    void SceneUI::onDraw()
    {
        if (_is_msg_hide)
            return;

        _display.setTextColor(TFT_CYAN);
        _display.setTextSize(2);
        _display.setTextFont(2);
        _display.drawString(STR_WAIT_CLIENTS, 25, 100);
    }

    void SceneUI::hideStartMsg()
    {
        _is_msg_hide = true;
    }
}