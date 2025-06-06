#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"

#include "meow/ui/widget/text/TextBox.h"
#include "meow/ui/widget/keyboard/Keyboard.h"

using namespace meow;

class PrefFileServerContext : public IContext
{
public:
    PrefFileServerContext();
    virtual ~PrefFileServerContext() {}

protected:
    virtual bool loop() override;
    virtual void update() override;

private:
    const uint16_t KEY_LOCK_DUR = 250;

    enum Widget_ID : uint8_t
    {
        ID_LBL_SSID = 2,
        ID_LBL_PWD,
        ID_TXT_SSID,
        ID_TXT_PWD,
        ID_KEYBOARD,
        ID_TXT_DIALOG
    };

    enum Mode : uint8_t
    {
        MODE_MAIN,
        MODE_DIALOG
    };

    Label *_ssid_lbl;
    TextBox *_ssid_txt;
    String _ssid_str;

    Label *_pwd_lbl;
    TextBox *_pwd_txt;
    String _pwd_str;

    Keyboard *_keyboard;

    TextBox *_dialog_txt;

    bool _is_ssid_edit = true;

    Mode _mode = MODE_MAIN;

    void showMainTmpl();
    void showDialogTmpl();
    void hideDialog(bool save_fields);
    void saveSettings();

    void up();
    void down();
    void left();
    void right();

    void ok();
    void savePressed();

    void back();
    void backPressed();

    void changeTbFocus();

    void keyboardClickHandler();
};