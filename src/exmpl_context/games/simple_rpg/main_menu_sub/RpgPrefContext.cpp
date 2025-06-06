#include "RpgPrefContext.h"
#include "meow/manager/settings/SettingsManager.h"

#include "../../../WidgetCreator.h"

#include "../../../resources/color.h"
#include "../../../resources/const.h"
#include "../../../resources/string.h"
#include "../res/string.h"

namespace simple_rpg
{
    void RpgPrefContext::showSDErrTmpl()
    {
        WidgetCreator creator;
        EmptyLayout *layout = creator.getEmptyLayout();

        Label *msg_lbl = creator.getStatusMsgLable(1, STR_SD_ERR);
        layout->addWidget(msg_lbl);

        _state_id = STATE_SD_UNMOUNT;

        setLayout(layout);
    }

    void RpgPrefContext::handleSDUnmountBtns()
    {
        if (_input.isReleased(BtnID::BTN_B))
        {
            _input.lock(BtnID::BTN_B, CLICK_LOCK);
            release();
        }
    }

    RpgPrefContext::RpgPrefContext()
    {
        WidgetCreator creator;
        EmptyLayout *layout = creator.getEmptyLayout();
        setLayout(layout);

        SettingsManager settings;
        if (!settings.isMounted()) // Якщо карту пам'яті не примонтовано
        {
            showSDErrTmpl(); // Виводимо шаблон з повідомлення про помилку
            return;
        }

        showMainMenuTmpl(); // Виводимо головне меню налаштувань
    }

    bool RpgPrefContext::loop()
    {
        return true;
    }

    void RpgPrefContext::update()
    {
        switch (_state_id)
        {
        case STATE_SD_UNMOUNT: // Якщо карту пам'яті не примонтовано
            handleSDUnmountBtns();
            break;
        case STATE_MAIN_MENU:
            handleMainMenuBtns();
            break;
        case STATE_PREF_NICK:
        case STATE_PREF_SERV_NAME:
        case STATE_PREF_SERV_PWD:
            handleDialBtns();
            break;
        }
    }

    void RpgPrefContext::showMainMenuTmpl()
    {
        _state_id = STATE_MAIN_MENU;

        WidgetCreator creator;
        EmptyLayout *layout = creator.getEmptyLayout();

        _pref_menu = new FixedMenu(ID_MENU_MAIN);
        layout->addWidget(_pref_menu);
        _pref_menu->setBackColor(COLOR_MAIN_BACK);
        _pref_menu->setWidth(D_WIDTH);
        _pref_menu->setHeight(D_HEIGHT - TFT_CUTOUT * 2);
        _pref_menu->setItemHeight(40);
        _pref_menu->setLoopState(true);
        _pref_menu->setPos(0, TFT_CUTOUT);

        // nick
        MenuItem *nick_item = creator.getMenuItem(ID_ITEM_NICK);
        _pref_menu->addItem(nick_item);
        nick_item->setBackColor(COLOR_MAIN_BACK);

        Label *nick_lbl = creator.getItemLabel(STR_NICKNAME);
        nick_item->setLbl(nick_lbl);

        // serv name
        MenuItem *serv_name_item = creator.getMenuItem(ID_ITEM_SERV_NAME);
        _pref_menu->addItem(serv_name_item);
        serv_name_item->setBackColor(COLOR_MAIN_BACK);

        Label *serv_name_lbl = creator.getItemLabel(STR_SERV_NAME);
        serv_name_item->setLbl(serv_name_lbl);

        // client name
        MenuItem *serv_pwd_item = creator.getMenuItem(ID_ITEM_SERV_PWD);
        _pref_menu->addItem(serv_pwd_item);
        serv_pwd_item->setBackColor(COLOR_MAIN_BACK);

        Label *serv_pwd_lbl = creator.getItemLabel(STR_SERV_PWD);
        serv_pwd_item->setLbl(serv_pwd_lbl);

        setLayout(layout);
    }

    void RpgPrefContext::handleMainMenuBtns()
    {
        if (_input.isHolded(BtnID::BTN_UP))
        {
            _input.lock(BtnID::BTN_UP, HOLD_LOCK);
            _pref_menu->focusUp();
        }
        else if (_input.isHolded(BtnID::BTN_DOWN))
        {
            _input.lock(BtnID::BTN_DOWN, HOLD_LOCK);
            _pref_menu->focusDown();
        }
        else if (_input.isReleased(BtnID::BTN_A))
        {
            _input.lock(BtnID::BTN_A, CLICK_LOCK);

            uint16_t id = _pref_menu->getCurrItemID();

            if (id == ID_ITEM_NICK)
                showPrefNickTmpl();
            else if (id == ID_ITEM_SERV_NAME)
                showPrefServNameTmpl();
            else if (id == ID_ITEM_SERV_PWD)
                showPrefServPwdTmpl();
        }
        else if (_input.isReleased(BtnID::BTN_B))
        {
            _input.lock(BtnID::BTN_B, CLICK_LOCK);
            release();
        }
    }

    void RpgPrefContext::showPrefNickTmpl()
    {
        _state_id = STATE_PREF_NICK;

        SettingsManager setting;
        String client_nick = setting.get(STR_TEST_GAME_NICK);

        WidgetCreator creator;
        EmptyLayout *layout = creator.getEmptyLayout();
        addDialog(layout, STR_ENTER_NICK, client_nick.c_str());
        setLayout(layout);
    }

    void RpgPrefContext::showPrefServNameTmpl()
    {
        _state_id = STATE_PREF_SERV_NAME;

        SettingsManager setting;
        String serv_ssid = setting.get(STR_TEST_GAME_S_NAME);

        WidgetCreator creator;
        EmptyLayout *layout = creator.getEmptyLayout();

        addDialog(layout, STR_ENTER_SERV_NAME, serv_ssid.c_str());

        setLayout(layout);
    }

    void RpgPrefContext::showPrefServPwdTmpl()
    {
        _state_id = STATE_PREF_SERV_PWD;

        SettingsManager setting;
        String serv_pwd = setting.get(STR_TEST_GAME_S_PWD);

        WidgetCreator creator;
        EmptyLayout *layout = creator.getEmptyLayout();

        addDialog(layout, STR_ENTER_SERV_PWD, serv_pwd.c_str());

        setLayout(layout);
    }

    void RpgPrefContext::handleDialBtns()
    {
        if (_input.isHolded(BtnID::BTN_UP))
        {
            _input.lock(BtnID::BTN_UP, CLICK_LOCK);
            _keyboard->focusUp();
        }
        else if (_input.isHolded(BtnID::BTN_DOWN))
        {
            _input.lock(BtnID::BTN_DOWN, CLICK_LOCK);
            _keyboard->focusDown();
        }
        else if (_input.isHolded(BtnID::BTN_LEFT))
        {
            _input.lock(BtnID::BTN_LEFT, CLICK_LOCK);
            _keyboard->focusLeft();
        }
        else if (_input.isHolded(BtnID::BTN_RIGHT))
        {
            _input.lock(BtnID::BTN_RIGHT, CLICK_LOCK);
            _keyboard->focusRight();
        }
        else if (_input.isReleased(BtnID::BTN_A))
        {
            _input.lock(BtnID::BTN_A, CLICK_LOCK);
            _dialog_txt->addChars(_keyboard->getCurrBtnTxt().c_str());
        }
        else if (_input.isReleased(BtnID::BTN_B))
        {
            _input.lock(BtnID::BTN_B, CLICK_LOCK);
            _dialog_txt->removeLastChar();
        }
        else if (_input.isPressed(BtnID::BTN_SAVE))
        {
            _input.lock(BtnID::BTN_SAVE, PRESS_LOCK);
            saveDialogResult();
        }
        else if (_input.isPressed(BtnID::BTN_EXIT))
        {
            _input.lock(BtnID::BTN_EXIT, PRESS_LOCK);
            showMainMenuTmpl();
        }
    }

    void RpgPrefContext::saveDialogResult()
    {
        SettingsManager settings;

        if (_state_id == STATE_PREF_NICK)
        {
            String client_nick = _dialog_txt->getText();
            settings.set(STR_TEST_GAME_NICK, client_nick.c_str());
        }
        else if (_state_id == STATE_PREF_SERV_NAME)
        {
            String serv_ssid = _dialog_txt->getText();
            settings.set(STR_TEST_GAME_S_NAME, serv_ssid.c_str());
        }
        else if (_state_id == STATE_PREF_SERV_PWD)
        {
            String serv_pwd = _dialog_txt->getText();
            settings.set(STR_TEST_GAME_S_PWD, serv_pwd.c_str());
        }

        showMainMenuTmpl();
    }

    void RpgPrefContext::addDialog(EmptyLayout *layout, const char *title_txt, const char *start_txt)
    {
        Label *title_lbl = new Label(ID_DIALOG_LBL);
        layout->addWidget(title_lbl);
        title_lbl->setText(title_txt);
        title_lbl->setAlign(IWidget::ALIGN_CENTER);
        title_lbl->setGravity(IWidget::GRAVITY_CENTER);
        title_lbl->setWidth(D_WIDTH);
        title_lbl->setBackColor(COLOR_MAIN_BACK);
        title_lbl->setTextColor(TFT_WHITE);
        title_lbl->setTicker(true);
        title_lbl->setPos(0, 3);

        _dialog_txt = new TextBox(ID_PWD_TXT);
        layout->addWidget(_dialog_txt);
        _dialog_txt->setText(start_txt);
        _dialog_txt->setTextOffset(5);
        _dialog_txt->setWidth(D_WIDTH - 10);
        _dialog_txt->setHeight(40);
        _dialog_txt->setBackColor(TFT_WHITE);
        _dialog_txt->setTextColor(TFT_BLACK);
        _dialog_txt->setFontID(2);
        _dialog_txt->setTextSize(2);
        _dialog_txt->setPos(5, TFT_CUTOUT);
        _dialog_txt->setCornerRadius(3);

        WidgetCreator creator;
        _keyboard = creator.getStandardEnKeyboard(ID_KEYBOARD);
        layout->addWidget(_keyboard);
    }
}