#include "PrefWatchContext.h"

#include "../WidgetCreator.h"
#include "meow/ui/widget/spinbox/SpinBox.h"

#include "../resources/string.h"
#include "../resources/color.h"
#include "../resources/const.h"

PrefWatchContext::PrefWatchContext()
{
    WidgetCreator creator;

    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);
    //
    SpinBox *hours = new SpinBox(ID_HOURS_SPIN);
    layout->addWidget(hours);
    hours->setHeight(34);
    hours->setWidth(7 * 10);
    hours->setBackColor(TFT_BLACK);
    hours->setTextColor(TFT_ORANGE);
    hours->setTextSize(2);
    hours->setFocusBorderColor(COLOR_LIME);
    hours->setChangingBorder(true);
    hours->setCornerRadius(5);
    hours->setMin(0);
    hours->setMax(23);
    hours->setValue(0);
    hours->setPos((_display.width() - (hours->getWidth() * 2 + 5)) * 0.5, 100);

    SpinBox *minutes = hours->clone(ID_MINUTES_SPIN);
    layout->addWidget(minutes);
    minutes->setMin(0);
    minutes->setMax(59);
    minutes->setValue(0);
    minutes->setPos(hours->getXPos() + hours->getWidth() + 5, hours->getYPos());

    SpinBox *day = hours->clone(ID_DAY_SPIN);
    layout->addWidget(day);
    day->setMin(1);
    day->setMax(31);
    day->setValue(0);
    day->setPos((_display.width() - (hours->getWidth() * 3 + 10)) * 0.5, hours->getYPos() + hours->getHeight() + 3);

    SpinBox *month = hours->clone(ID_MONTH_SPIN);
    layout->addWidget(month);
    month->setMin(1);
    month->setMax(12);
    month->setValue(0);
    month->setPos(day->getXPos() + day->getWidth() + 5, day->getYPos());

    SpinBox *year = hours->clone(ID_YEAR_SPIN);
    layout->addWidget(year);
    year->setMin(2020);
    year->setMax(3000);
    year->setValue(0);
    year->setPos(month->getXPos() + month->getWidth() + 5, day->getYPos());

    hours->setFocus();

    //
    Label *header_lbl = new Label(ID_HEADER);
    layout->addWidget(header_lbl);
    header_lbl->setText(STR_WATCH);
    header_lbl->setWidth(_display.width());
    header_lbl->setBackColor(COLOR_MAIN_BACK);
    header_lbl->setTextColor(TFT_WHITE);
    header_lbl->setAlign(IWidget::ALIGN_CENTER);
    header_lbl->setGravity(IWidget::GRAVITY_CENTER);
}

bool PrefWatchContext::loop()
{
    return true;
}

void PrefWatchContext::update()
{
    if (_input.isPressed(BtnID::BTN_EXIT))
    {
        _input.lock(BtnID::BTN_EXIT, PRESS_LOCK);
        openContextByID(ID_CONTEXT_PREF_SEL);
    }
    else if (_input.isPressed(BtnID::BTN_SAVE))
    {
        _input.lock(BtnID::BTN_SAVE, PRESS_LOCK);
        openContextByID(ID_CONTEXT_PREF_SEL);
    }
    else if (_input.isReleased(BtnID::BTN_B))
    {
        _input.lock(BtnID::BTN_B, PRESS_LOCK);
        openContextByID(ID_CONTEXT_PREF_SEL);
    }
    else if (_input.isReleased(BtnID::BTN_LEFT))
    {
        _input.lock(BtnID::BTN_LEFT, CLICK_LOCK);

        SpinBox *spinBox = (SpinBox *)getLayout()->getWidgetByIndx(_curent_focus_pos);
        spinBox->removeFocus();

        if (_curent_focus_pos > 0)
            --_curent_focus_pos;
        else
            _curent_focus_pos = MAX_SPIN_POS;

        spinBox = (SpinBox *)getLayout()->getWidgetByIndx(_curent_focus_pos);
        spinBox->setFocus();
    }
    else if (_input.isReleased(BtnID::BTN_RIGHT))
    {
        _input.lock(BtnID::BTN_RIGHT, CLICK_LOCK);

        SpinBox *spinBox = (SpinBox *)getLayout()->getWidgetByIndx(_curent_focus_pos);
        spinBox->removeFocus();

        if (_curent_focus_pos == MAX_SPIN_POS)
            _curent_focus_pos = 0;
        else
            ++_curent_focus_pos;

        spinBox = (SpinBox *)getLayout()->getWidgetByIndx(_curent_focus_pos);
        spinBox->setFocus();
    }
    else if (_input.isHolded(BtnID::BTN_UP))
    {
        _input.lock(BtnID::BTN_UP, HOLD_LOCK);
        SpinBox *spinBox = (SpinBox *)getLayout()->getWidgetByIndx(_curent_focus_pos);
        spinBox->up();
    }
    else if (_input.isHolded(BtnID::BTN_DOWN))
    {
        _input.lock(BtnID::BTN_DOWN, HOLD_LOCK);
        SpinBox *spinBox = (SpinBox *)getLayout()->getWidgetByIndx(_curent_focus_pos);
        spinBox->down();
    }
}