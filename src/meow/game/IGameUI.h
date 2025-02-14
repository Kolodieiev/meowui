#pragma once
#include <Arduino.h>
#include "../driver/graphics/GraphicsDriver.h"

namespace meow
{
    class IGameUI
    {
    public:
        IGameUI(GraphicsDriver &display) : _display{display} {}
        virtual ~IGameUI() {}
        virtual void onDraw() = 0;
        IGameUI(const IGameUI &rhs) = delete;
        IGameUI &operator=(const IGameUI &rhs) = delete;

    protected:
        GraphicsDriver &_display;
    };
}