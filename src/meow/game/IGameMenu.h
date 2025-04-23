#pragma once
#include <Arduino.h>
#include "../ui/widget/menu/FixedMenu.h"
#include "../ui/widget/layout/EmptyLayout.h"

namespace meow
{
    class IGameMenu
    {
    public:
        IGameMenu()
        {
            try
            {
                _menu = new FixedMenu(1);
                _menu_back = new EmptyLayout(1);
                _menu_back->addWidget(_menu);
            }
            catch (const std::bad_alloc &e)
            {
                log_e("%s", e.what());
                esp_restart();
            }
        }

        virtual ~IGameMenu()
        {
            delete _menu_back;
        }

        virtual void onDraw() = 0;
        IGameMenu(const IGameMenu &rhs) = delete;
        IGameMenu &operator=(const IGameMenu &rhs) = delete;

        virtual void focusUp() = 0;
        virtual void focusDown() = 0;

        uint16_t getCurrentItemID() const { return _menu->getCurrentItemID(); }

    protected:
        FixedMenu *_menu;
        EmptyLayout *_menu_back;
    };

}