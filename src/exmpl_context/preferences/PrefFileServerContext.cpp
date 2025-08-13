#include "PrefFileServerContext.h"
#include "meow/manager/settings/SettingsManager.h"

#include "../WidgetCreator.h"

bool PrefFileServerContext::loop()
{
    return true;
}

PrefFileServerContext::PrefFileServerContext()
{
    SettingsManager settings;
    _ssid_str = settings.get(STR_PREF_FS_AP_SSID);
    if (_ssid_str.isEmpty())
        _ssid_str = STR_DEF_SSID;

    _pwd_str = settings.get(STR_PREF_FS_AP_PWD);
    if (_pwd_str.isEmpty())
        _pwd_str = STR_DEF_PWD;

    showMainTmpl();
}

void PrefFileServerContext::showMainTmpl()
{
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    //----------------------------------------------------

    const uint8_t LEFT_PADDING = 5;
    const uint8_t SPACE_BETWEEN = 40;

    _ssid_lbl = new Label(ID_LBL_SSID);
    layout->addWidget(_ssid_lbl);
    _ssid_lbl->setText(STR_AP_NAME);
    _ssid_lbl->setPos(LEFT_PADDING, TFT_CUTOUT);
    _ssid_lbl->setBackColor(COLOR_MAIN_BACK);
    _ssid_lbl->initWidthToFit();

    _ssid_txt = new TextBox(ID_TXT_SSID);
    layout->addWidget(_ssid_txt);
    _ssid_txt->setText(_ssid_str);
    _ssid_txt->setTextOffset(5);
    _ssid_txt->setFocusBorderColor(TFT_ORANGE);
    _ssid_txt->setChangingBorder(true);
    _ssid_txt->setTextSize(2);
    _ssid_txt->setWidth(D_WIDTH - LEFT_PADDING * 2);
    _ssid_txt->setPos(LEFT_PADDING, _ssid_lbl->getYPos() + _ssid_lbl->getHeight() + 1);

    //----------------------------------------------------

    _pwd_lbl = _ssid_lbl->clone(ID_LBL_PWD);
    layout->addWidget(_pwd_lbl);
    _pwd_lbl->setText(STR_AP_PWD);
    _pwd_lbl->initWidthToFit();
    _pwd_lbl->setPos(LEFT_PADDING, _ssid_txt->getYPos() + _ssid_txt->getHeight() + SPACE_BETWEEN);

    _pwd_txt = _ssid_txt->clone(ID_TXT_PWD);
    layout->addWidget(_pwd_txt);
    _pwd_txt->setType(TextBox::TYPE_PASSWORD);
    _pwd_txt->setText(_pwd_str);
    _pwd_txt->setPos(LEFT_PADDING, _pwd_lbl->getYPos() + _pwd_lbl->getHeight() + 1);

    if (_is_ssid_edit)
    {
        _ssid_lbl->setTextColor(TFT_ORANGE);
        _pwd_lbl->setTextColor(TFT_WHITE);

        _ssid_txt->setFocus();
        _pwd_txt->removeFocus();
    }
    else
    {
        _ssid_lbl->setTextColor(TFT_WHITE);
        _pwd_lbl->setTextColor(TFT_ORANGE);

        _ssid_txt->removeFocus();
        _pwd_txt->setFocus();
    }

    _mode = MODE_MAIN;
}

void PrefFileServerContext::showDialogTmpl()
{
    IWidgetContainer *layout = getLayout();
    layout->disable();
    layout->delWidgets();
    layout->setBackColor(TFT_BLACK);

    WidgetCreator creator;

    _dialog_txt = new TextBox(ID_TXT_DIALOG);
    layout->addWidget(_dialog_txt);
    _dialog_txt->setTextOffset(5);
    _dialog_txt->setWidth(_display.width() - 10);
    _dialog_txt->setHeight(40);
    _dialog_txt->setBackColor(TFT_WHITE);
    _dialog_txt->setTextColor(TFT_BLACK);
    _dialog_txt->setFontID(2);
    _dialog_txt->setTextSize(2);
    _dialog_txt->setPos(5, TFT_CUTOUT);
    _dialog_txt->setCornerRadius(3);

    if (_is_ssid_edit)
        _dialog_txt->setText(_ssid_str);
    else
        _dialog_txt->setText(_pwd_str);

    _keyboard = creator.getStandardEnKeyboard(ID_KEYBOARD);
    layout->addWidget(_keyboard);

    _mode = MODE_DIALOG;

    layout->enable();
}

void PrefFileServerContext::hideDialog(bool save_fields)
{
    if (save_fields)
    {
        String dialog_res_str = _dialog_txt->getText();

        if (_is_ssid_edit)
            _ssid_str = dialog_res_str.isEmpty() ? STR_DEF_SSID : dialog_res_str;
        else
            _pwd_str = dialog_res_str.isEmpty() ? STR_DEF_PWD : dialog_res_str;

        saveSettings();
    }

    showMainTmpl();
}

void PrefFileServerContext::saveSettings()
{
    SettingsManager settings;
    settings.set(STR_PREF_FS_AP_SSID, _ssid_str.c_str());
    settings.set(STR_PREF_FS_AP_PWD, _pwd_str.c_str());
}

void PrefFileServerContext::up()
{
    if (_mode == MODE_MAIN)
        changeTbFocus();
    else
        _keyboard->focusUp();
}

void PrefFileServerContext::down()
{
    if (_mode == MODE_MAIN)
        changeTbFocus();
    else
        _keyboard->focusDown();
}

void PrefFileServerContext::left()
{
    if (_mode == MODE_DIALOG)
        _keyboard->focusLeft();
}

void PrefFileServerContext::right()
{
    if (_mode == MODE_DIALOG)
        _keyboard->focusRight();
}

void PrefFileServerContext::savePressed()
{
    if (_mode == MODE_DIALOG)
    {
        hideDialog(true);
    }
}

void PrefFileServerContext::ok()
{
    if (_mode == MODE_MAIN)
        showDialogTmpl();
    else if (_mode == MODE_DIALOG)
        keyboardClickHandler();
}

void PrefFileServerContext::keyboardClickHandler()
{
    _dialog_txt->addChars(_keyboard->getCurrBtnTxt().c_str());
}

void PrefFileServerContext::back()
{
    if (_mode == MODE_MAIN)
        openContextByID(ID_CONTEXT_PREF_SEL);
    else if (_mode == MODE_DIALOG)
        _dialog_txt->removeLastChar();
}

void PrefFileServerContext::backPressed()
{
    hideDialog(false);
}

void PrefFileServerContext::changeTbFocus()
{
    if (_is_ssid_edit)
    {
        _ssid_lbl->setTextColor(TFT_WHITE);
        _pwd_lbl->setTextColor(TFT_ORANGE);

        _ssid_txt->removeFocus();
        _pwd_txt->setFocus();
    }
    else
    {
        _ssid_lbl->setTextColor(TFT_ORANGE);
        _pwd_lbl->setTextColor(TFT_WHITE);

        _ssid_txt->setFocus();
        _pwd_txt->removeFocus();
    }

    _is_ssid_edit = !_is_ssid_edit;
}

void PrefFileServerContext::update()
{
    if (_input.isPressed(BtnID::BTN_SAVE))
    {
        _input.lock(BtnID::BTN_SAVE, 1500);
        savePressed();
    }
    else if (_input.isPressed(BtnID::BTN_EXIT))
    {
        _input.lock(BtnID::BTN_EXIT, 1500);
        backPressed();
    }
    else if (_input.isHolded(BtnID::BTN_UP))
    {
        _input.lock(BtnID::BTN_UP, KEY_LOCK_DUR);
        up();
    }
    else if (_input.isHolded(BtnID::BTN_DOWN))
    {
        _input.lock(BtnID::BTN_DOWN, KEY_LOCK_DUR);
        down();
    }
    else if (_input.isHolded(BtnID::BTN_RIGHT))
    {
        _input.lock(BtnID::BTN_RIGHT, KEY_LOCK_DUR);
        right();
    }
    else if (_input.isHolded(BtnID::BTN_LEFT))
    {
        _input.lock(BtnID::BTN_LEFT, KEY_LOCK_DUR);
        left();
    }
    else if (_input.isReleased(BtnID::BTN_A))
    {
        _input.lock(BtnID::BTN_A, KEY_LOCK_DUR);
        ok();
    }
    else if (_input.isReleased(BtnID::BTN_B))
    {
        _input.lock(BtnID::BTN_B, KEY_LOCK_DUR);
        back();
    }
}
