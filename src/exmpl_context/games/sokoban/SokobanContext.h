#pragma once
#include <Arduino.h>
//
#include "meow/game/DataStream.h"
//
#include "meow/ui/context/IContext.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/ui/widget/scrollbar/ScrollBar.h"
#include "meow/game/IGameScene.h"

using namespace meow;

namespace sokoban
{
    class SokobanContext : public IContext
    {
    public:
        SokobanContext();
        virtual ~SokobanContext() {}

    protected:
        virtual bool loop() override;
        virtual void update() override;

    private:
        const uint8_t MENU_ITEMS_NUM{10};
        const uint8_t LEVEL_NUM{70};

        enum Widget_ID : uint8_t
        {
            ID_LVL_LIST = 1,
            ID_SCROLL
        };

        bool _is_game_started{false};

        FixedMenu *_lvl_menu;
        ScrollBar *_scrollbar;

        IGameScene *_scene;
        DataStream _stored_objs{1024};

        void showLvlMenu();
    };
}