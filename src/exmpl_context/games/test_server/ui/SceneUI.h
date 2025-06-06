#pragma once
#include <Arduino.h>

#include "meow/game/IGameUI.h"
#include "meow/game/object/IGameObject.h"

using namespace meow;

namespace test_server
{
    class SceneUI : public IGameUI
    {
    public:
        SceneUI();
        virtual ~SceneUI() {}
        virtual void onDraw() override;
        void hideStartMsg();

    private:
        bool _is_msg_hide{false};
    };
}