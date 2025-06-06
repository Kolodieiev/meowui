#include "PrefBrightContext.h"

#include "meow/manager/settings/SettingsManager.h"

#include "../WidgetCreator.h"

#include "../resources/string.h"
#include "../resources/color.h"
#include "../resources/const.h"

PrefBrightContext::PrefBrightContext()
{
    WidgetCreator creator;

    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);
    //
    Label *header_lbl = new Label(ID_HEADER);
    layout->addWidget(header_lbl);
    header_lbl->setText(STR_BRIGHT);
    header_lbl->setWidth(_display.width());
    header_lbl->setBackColor(COLOR_MAIN_BACK);
    header_lbl->setTextColor(TFT_WHITE);
    header_lbl->setAlign(IWidget::ALIGN_CENTER);
    header_lbl->setGravity(IWidget::GRAVITY_CENTER);
    //
    _progress = new ProgressBar(ID_PROGRESSBAR);
    layout->addWidget(_progress);
    _progress->setBackColor(TFT_BLACK);
    _progress->setProgressColor(TFT_ORANGE);
    _progress->setBorderColor(TFT_WHITE);
    _progress->setMax(MAX_BRIGHT);
    _progress->setWidth(_display.width() - 5 * 8);
    _progress->setHeight(20);
    _progress->setProgress(_old_bright);
    _progress->setPos((_display.width() - _progress->getWidth()) / 2, 40);
}

bool PrefBrightContext::loop()
{
    return true;
}

void PrefBrightContext::update()
{
    if (_input.isPressed(BtnID::BTN_SAVE))
    {
        _input.lock(BtnID::BTN_SAVE, 1000);

        SettingsManager settings;
        settings.set(STR_PREF_BRIGHT, String(_progress->getProgress()).c_str());
        openContextByID(ID_CONTEXT_PREF_SEL);
    }
    else if (_input.isReleased(BtnID::BTN_B))
    {
        _input.lock(BtnID::BTN_B, 500);
        openContextByID(ID_CONTEXT_PREF_SEL);
    }
    else if (_input.isHolded(BtnID::BTN_RIGHT))
    {
        _input.lock(BtnID::BTN_RIGHT, 100);

        uint16_t cur_progress = _progress->getProgress();

        if (cur_progress < MAX_BRIGHT)
        {
            cur_progress += BRIGHT_STEP;
            _progress->setProgress(cur_progress);
        }
    }
    else if (_input.isHolded(BtnID::BTN_LEFT))
    {
        _input.lock(BtnID::BTN_LEFT, 100);

        uint16_t cur_progress = _progress->getProgress();
        if (cur_progress > BRIGHT_STEP * 2)
        {
            cur_progress -= BRIGHT_STEP;
            _progress->setProgress(cur_progress);
        }
    }
}