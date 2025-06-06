#pragma once
#include <Arduino.h>

#include "meow/game/IGameUI.h"
#include "meow/ui/widget/text/Label.h"

using namespace meow;

namespace simple_rpg
{
    class BaseUI : public IGameUI
    {
    public:
        BaseUI();
        virtual ~BaseUI() { delete _bunny_counter_lbl; }

        virtual void onDraw() override;
        void updBunnyNum(uint16_t value);

    private:
        Label *_bunny_counter_lbl{nullptr};
    };
}