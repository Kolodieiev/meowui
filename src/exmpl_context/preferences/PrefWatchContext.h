#pragma once
#include <Arduino.h>

#include "meow/ui/context/IContext.h"

using namespace meow;

class PrefWatchContext : public IContext
{

public:
    PrefWatchContext();
    virtual ~PrefWatchContext() {}

protected:
    virtual bool loop() override;
    virtual void update() override;

private:
    enum Widget_ID : uint8_t
    {
        ID_HEADER = 2,
        // Watch
        ID_HOURS_SPIN,
        ID_MINUTES_SPIN,
        ID_DAY_SPIN,
        ID_MONTH_SPIN,
        ID_YEAR_SPIN,
    };

    const uint8_t MAX_SPIN_POS{4};

    uint8_t _curent_focus_pos{0};
};
