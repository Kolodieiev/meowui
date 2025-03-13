#pragma once
#include <Arduino.h>

#include "meow/ui/context/IContext.h"

using namespace meow;

class SplashContext : public IContext
{
public:
    SplashContext(GraphicsDriver &display);
    virtual ~SplashContext(){};

protected:
    virtual bool loop() override;
    virtual void update() override;
    //
private:
    const uint16_t SHOWING_INIT_TIME{1000};

    unsigned long _start_time;

    void addLabel(uint16_t x_pos, uint16_t y_pos, const char *res_str, uint16_t color);

    uint8_t _widget_id{1};
};