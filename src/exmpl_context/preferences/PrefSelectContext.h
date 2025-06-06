#pragma once
#include <Arduino.h>
#include "meow/manager/settings/SettingsManager.h"

#include "meow/ui/context/IContext.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/ui/widget/scrollbar/ScrollBar.h"

using namespace meow;

class PrefSelectContext : public IContext
{

public:
    PrefSelectContext();
    virtual ~PrefSelectContext() {}

protected:
    virtual bool loop() override;
    virtual void update() override;

private:
    enum Widget_ID : uint8_t
    {
        ID_MENU = 2,
        ID_SCROLLBAR,
        ID_TOGGLE,
        ID_ERR_LBL,
    };

    enum ItemID : uint8_t
    {
        ITEM_ID_BRIGHT = 1,
        ITEM_ID_AUDIO_MONO,
        ITEM_ID_WATCH,
        ITEM_ID_FILE_SERVER,
    };

    enum Mode : uint8_t
    {
        MODE_NORMAL = 0,
        MODE_SD_UNCONN,
    };

    SettingsManager _settings;

    Mode _mode = MODE_NORMAL;

    FixedMenu *_menu;
    ScrollBar *_scrollbar;

    void ok();
    void showSDErrTmpl();
};