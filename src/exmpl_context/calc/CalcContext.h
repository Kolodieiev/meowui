#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"

using namespace meow;

class CalcContext : public IContext
{
public:
    CalcContext();
    virtual ~CalcContext() {}

protected:
    virtual bool loop() override;
    virtual void update() override;
};