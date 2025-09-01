#include "WiFiContext.h"
//

#include "./res/ico_connect.h"

#include "../WidgetCreator.h"
#include "meow/ui/widget/layout/EmptyLayout.h"
#include "meow/ui/widget/toggle/ToggleSwitch.h"
#include "meow/ui/widget/menu/item/ToggleItem.h"

const char STR_TRANSMITTER_STATE[] = "Стан модуля WIFi";
const char STR_START_SCAN[] = "Розпочато скануваня";
const char STR_START_SCAN_ERR[] = "Помилка сканування";
const char STR_WIFI_BUSY[] = "Модуль зайнятий";
const char STR_DISCONNECT[] = "Від'єднaтися";
const char STR_FORGET[] = "Забути";
const char STR_CONNECTING[] = "З'єднання...";
const char STR_CONNECT_ERR[] = "Помилка з'єднання";

const char STR_WIFI_SUBDIR[] = "wifi";

WiFiContext::WiFiContext()
{
    if (!_sd.isMounted())
    {
        showSDErrTmpl();
        return;
    }

    showMainTmpl();
}

WiFiContext::~WiFiContext()
{
}

bool WiFiContext::loop()
{
    return true;
}

void WiFiContext::update()
{
    if (_input.isReleased(BtnID::BTN_UP))
    {
        _input.lock(BtnID::BTN_UP, CLICK_LOCK);
        up();
    }
    else if (_input.isReleased(BtnID::BTN_DOWN))
    {
        _input.lock(BtnID::BTN_DOWN, CLICK_LOCK);
        down();
    }
    else if (_input.isReleased(BtnID::BTN_LEFT))
    {
        _input.lock(BtnID::BTN_LEFT, CLICK_LOCK);
        left();
    }
    else if (_input.isReleased(BtnID::BTN_RIGHT))
    {
        _input.lock(BtnID::BTN_RIGHT, CLICK_LOCK);
        right();
    }
    else if (_input.isReleased(BtnID::BTN_A))
    {
        _input.lock(BtnID::BTN_A, CLICK_LOCK);
        ok();
    }
    else if (_input.isReleased(BtnID::BTN_D))
    {
        _input.lock(BtnID::BTN_D, CLICK_LOCK);
        showContextMenuTmpl();
    }
    else if (_input.isReleased(BtnID::BTN_C))
    {
        _input.lock(BtnID::BTN_C, CLICK_LOCK);
        changeKbCaps();
    }
    else if (_input.isReleased(BtnID::BTN_B))
    {
        _input.lock(BtnID::BTN_B, CLICK_LOCK);
        back();
    }
    else if (_input.isPressed(BtnID::BTN_SAVE))
    {
        _input.lock(BtnID::BTN_SAVE, PRESS_LOCK);
        savePressed();
    }
    else if (_input.isPressed(BtnID::BTN_EXIT))
    {
        _input.lock(BtnID::BTN_EXIT, PRESS_LOCK);
        exitPressed();
    }
}

void WiFiContext::showSDErrTmpl()
{
    _mode = MODE_SD_UNCONN;
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    Label *err_lbl = new Label(ID_ERR_LBL);
    layout->addWidget(err_lbl);
    err_lbl->setText(STR_SD_ERR);
    err_lbl->setAlign(IWidget::ALIGN_CENTER);
    err_lbl->setGravity(IWidget::GRAVITY_CENTER);
    err_lbl->setBackColor(COLOR_MAIN_BACK);
    err_lbl->setWidth(D_WIDTH);
    err_lbl->setHeight(D_HEIGHT);
}

void WiFiContext::showMainTmpl()
{
    _mode = MODE_MAIN;
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    _main_menu = new FixedMenu(ID_MAIN_MENU);
    layout->addWidget(_main_menu);
    _main_menu->setWidth(D_WIDTH);
    _main_menu->setHeight(D_HEIGHT - TFT_CUTOUT * 2);
    _main_menu->setPos(0, TFT_CUTOUT);
    _main_menu->setItemHeight(40);
    _main_menu->setLoopState(true);

    // Add state item
    ToggleItem *wifi_state_item = new ToggleItem(ID_ITEM_WIFI_STATE);
    _main_menu->addItem(wifi_state_item);
    wifi_state_item->setFocusBorderColor(COLOR_LIME);
    wifi_state_item->setFocusBackColor(COLOR_FOCUS_BACK);
    wifi_state_item->setChangingBorder(true);
    wifi_state_item->setChangingBack(true);

    Label *wifi_state_lbl = creator.getItemLabel(STR_TRANSMITTER_STATE, 4, 2);
    wifi_state_item->setLbl(wifi_state_lbl);

    ToggleSwitch *wifi_state_toggle = new ToggleSwitch(1);
    wifi_state_item->setToggle(wifi_state_toggle);
    wifi_state_toggle->setWidth(40);
    wifi_state_toggle->setHeight(20);
    wifi_state_toggle->setCornerRadius(7);

    if (_wifi.isEnabled())
    {
        addCurrNetItem();
        wifi_state_toggle->setOn(true);
        loadNetsList();
    }
    else
    {
        wifi_state_toggle->setOn(false);
    }
}

void WiFiContext::showEnterPwdTmpl()
{
    _mode = MODE_ENTER_PWD;
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    _pwd_txt = new TextBox(ID_PWD_TXT);
    layout->addWidget(_pwd_txt);
    _pwd_txt->setTextOffset(5);
    _pwd_txt->setWidth(_display.width() - 10);
    _pwd_txt->setHeight(40);
    _pwd_txt->setBackColor(TFT_WHITE);
    _pwd_txt->setTextColor(TFT_BLACK);
    _pwd_txt->setFontID(2);
    _pwd_txt->setTextSize(2);
    _pwd_txt->setPos(5, TFT_CUTOUT);
    _pwd_txt->setCornerRadius(3);

    _keyboard = creator.getStandardEnKeyboard(ID_KEYBOARD);
    layout->addWidget(_keyboard);
}

void WiFiContext::addCurrNetItem()
{
    if (!_wifi.isConnected())
        return;

    String ssid_name = _wifi.getSSID();

    WidgetCreator creator;
    MenuItem *cur_net_item = creator.getMenuItem(ID_ITEM_CUR_NET);
    _main_menu->addItem(cur_net_item);

    Image *conn_ico = new Image(1);
    cur_net_item->setImg(conn_ico);
    conn_ico->init(16, 16);
    conn_ico->setCornerRadius(1);
    conn_ico->setTranspColor(TFT_TRANSPARENT);
    conn_ico->setSrc(ICO_CONNECT);

    Label *conn_lbl = creator.getItemLabel(ssid_name.c_str(), 4, 2);
    cur_net_item->setLbl(conn_lbl);
    conn_lbl->setTickerInFocus(true);
}

void WiFiContext::up()
{
    if (_mode == MODE_ENTER_PWD)
    {
        _keyboard->focusUp();
    }
    else if (_mode == MODE_MAIN)
    {
        _main_menu->focusUp();
    }
    else if (_mode == MODE_CONTEXT_MENU)
    {
        _context_menu->focusUp();
    }
}

void WiFiContext::down()
{
    if (_mode == MODE_ENTER_PWD)
    {
        _keyboard->focusDown();
    }
    else if (_mode == MODE_MAIN)
    {
        _main_menu->focusDown();
    }
    else if (_mode == MODE_CONTEXT_MENU)
    {
        _context_menu->focusDown();
    }
}

void WiFiContext::left()
{
    if (_mode == MODE_ENTER_PWD)
    {
        _keyboard->focusLeft();
    }
}

void WiFiContext::right()
{
    if (_mode == MODE_ENTER_PWD)
    {
        _keyboard->focusRight();
    }
}

void WiFiContext::ok()
{
    if (_mode == MODE_ENTER_PWD)
    {
        _pwd_txt->addChars(_keyboard->getCurrBtnTxt().c_str());
    }
    else if (_mode == MODE_MAIN)
    {
        uint16_t item_id = _main_menu->getCurrItemID();

        if (item_id == ID_ITEM_WIFI_STATE)
        {

            ToggleItem *wifi_state_item = _main_menu->getWidgetByID(ID_ITEM_WIFI_STATE)->castTo<ToggleItem>();
            bool was_enabled = _wifi.isEnabled();

            if (!_wifi.toggle())
            {
                showToast(STR_WIFI_BUSY, TOAST_LENGTH_SHORT);
            }
            else if (!_wifi.isEnabled())
            {
                if (!was_enabled)
                {
                    showToast(STR_FAIL, TOAST_LENGTH_SHORT);
                }
                else
                {
                    wifi_state_item->setOn(false);
                    ToggleItem *temp_toggle = wifi_state_item->clone(ID_ITEM_WIFI_STATE);
                    _main_menu->delWidgets();
                    _main_menu->addItem(temp_toggle);
                }
            }
            else
            {
                wifi_state_item->setOn(true);
                loadNetsList();
            }
        }
        else if (item_id != ID_ITEM_CUR_NET)
        {
            String ssid = _main_menu->getCurrItemText();
            connectToNet(ssid);
        }
    }
    else if (_mode == MODE_CONTEXT_MENU)
    {
        uint16_t ctx_item_id = _context_menu->getCurrItemID();
        if (ctx_item_id == ID_ITEM_DISCONN)
        {
            _wifi.disconnect();
            ToggleItem *wifi_state_item = _main_menu->getWidgetByID(ID_ITEM_WIFI_STATE)->castTo<ToggleItem>();
            ToggleItem *temp_toggle = wifi_state_item->clone(ID_ITEM_WIFI_STATE);
            _main_menu->delWidgets();
            _main_menu->addItem(temp_toggle);
            hideContextMenu();
            loadNetsList();
        }
        else if (ctx_item_id == ID_ITEM_FORGET)
        {
            String path_to_pwd = _settings.getSettingsFilePath(_main_menu->getCurrItemText().c_str(), STR_WIFI_SUBDIR);
            if (!_fs.rmFile(path_to_pwd.c_str(), true))
                showToast(STR_FAIL);
            else
                showToast(STR_SUCCSESS);

            hideContextMenu();
        }
    }
}

void WiFiContext::back()
{
    if (_mode == MODE_ENTER_PWD)
    {
        _pwd_txt->removeLastChar();
    }
    else if (_mode == MODE_SD_UNCONN || _mode == MODE_MAIN)
    {
        _wifi.setScanDoneHandler(nullptr, nullptr);
        _wifi.setConnectDoneHandler(nullptr, nullptr);
        openContextByID(ID_CONTEXT_MENU);
    }
    else if (_mode == MODE_CONTEXT_MENU)
    {
        hideContextMenu();
    }
}

void WiFiContext::showContextMenuTmpl()
{
    if (_mode != MODE_MAIN)
        return;

    uint16_t item_id = _main_menu->getCurrItemID();
    if (item_id == ID_ITEM_WIFI_STATE)
        return;

    WidgetCreator creator;

    _context_menu = new FixedMenu(ID_CTX_MENU);
    _context_menu->setBackColor(COLOR_MENU_ITEM);
    _context_menu->setBorderColor(TFT_ORANGE);
    _context_menu->setBorder(true);
    _context_menu->setItemHeight(20);
    _context_menu->setWidth(120);
    _context_menu->setHeight(44);
    _context_menu->setPos(D_WIDTH - _context_menu->getWidth(), D_HEIGHT - _context_menu->getHeight() - TFT_CUTOUT);

    if (item_id == ID_ITEM_CUR_NET)
    {
        MenuItem *disconn_item = creator.getMenuItem(ID_ITEM_DISCONN);
        _context_menu->addItem(disconn_item);

        Label *disconn_lbl = creator.getItemLabel(STR_DISCONNECT, 4, 1);
        disconn_item->setLbl(disconn_lbl);
    }

    String wifi_pass = _settings.get(_main_menu->getCurrItemText().c_str(), STR_WIFI_SUBDIR);

    if (!wifi_pass.isEmpty())
    {
        MenuItem *forget_item = creator.getMenuItem(ID_ITEM_FORGET);
        _context_menu->addItem(forget_item);

        Label *forget_lbl = creator.getItemLabel(STR_FORGET, 4, 1);
        forget_item->setLbl(forget_lbl);
    }

    if (_context_menu->getSize() == 0)
    {
        delete _context_menu;
    }
    else
    {
        _main_menu->disable();
        _mode = MODE_CONTEXT_MENU;
        getLayout()->addWidget(_context_menu);
    }
}

void WiFiContext::hideContextMenu()
{
    if (_mode != MODE_CONTEXT_MENU)
        return;

    _mode = MODE_MAIN;
    getLayout()->delWidgetByID(ID_CTX_MENU);
    _main_menu->enable();
}

void WiFiContext::changeKbCaps()
{
    if (_mode == MODE_ENTER_PWD)
    {
        _pwd_kb_x_pos = _keyboard->getFocusXPos();
        _pwd_kb_y_pos = _keyboard->getFocusYPos();

        getLayout()->delWidgetByID(ID_KEYBOARD);

        WidgetCreator creator;
        _is_standrad_kb = !_is_standrad_kb;
        if (_is_standrad_kb)
            _keyboard = creator.getStandardEnKeyboard(ID_KEYBOARD);
        else
            _keyboard = creator.getCapsdEnKeyboard(ID_KEYBOARD);

        _keyboard->setFocusPos(_pwd_kb_x_pos, _pwd_kb_y_pos);
        getLayout()->addWidget(_keyboard);
    }
}

void WiFiContext::savePressed()
{
    if (_mode == MODE_ENTER_PWD)
    {
        if (_settings.set(_sel_ssid.c_str(), _pwd_txt->getText().c_str(), STR_WIFI_SUBDIR))
            connectToNet(_sel_ssid);
        else
            showToast(STR_FAIL, TOAST_LENGTH_LONG);
    }
}

void WiFiContext::exitPressed()
{
    if (_mode == MODE_ENTER_PWD)
    {
        showMainTmpl();
    }
    else if (_mode == MODE_MAIN || _mode == MODE_CONTEXT_MENU)
    {
        back();
    }
}

void WiFiContext::loadNetsList()
{
    _wifi.setScanDoneHandler(scanDoneHandler, this);
    if (!_wifi.startScan())
        showToast(STR_START_SCAN_ERR, TOAST_LENGTH_SHORT);
    else
        showToast(STR_START_SCAN, TOAST_LENGTH_SHORT);
}

void WiFiContext::updateNetList(bool no_scan)
{
    if (_mode != MODE_MAIN)
    {
        showMainTmpl();
        return;
    }

    takeLayoutMutex();

    WidgetCreator creator;

    ToggleItem *wifi_state_item = _main_menu->getWidgetByID(ID_ITEM_WIFI_STATE)->castTo<ToggleItem>();
    ToggleItem *temp_toggle = wifi_state_item->clone(ID_ITEM_WIFI_STATE);
    _main_menu->delWidgets();
    _main_menu->addItem(temp_toggle);

    addCurrNetItem();

    bool is_connected = _wifi.isConnected();
    bool curr_scipped = false;
    String cur_ssid = _wifi.getSSID();

    if (!no_scan)
        _wifi.getScanResult(_ssids);

    uint16_t i = ID_ITEM_CUR_NET + 1;
    for (auto i_b = _ssids.begin(), i_e = _ssids.end(); i_b < i_e; ++i_b)
    {
        if (!curr_scipped && is_connected && (i_b)->equals(cur_ssid))
        {
            curr_scipped = true;
            continue;
        }

        MenuItem *item = creator.getMenuItem(i);
        _main_menu->addItem(item);

        Label *item_lbl = creator.getItemLabel((i_b)->c_str());
        item->setLbl(item_lbl);
        ++i;
    }

    giveLayoutMutex();
}

void WiFiContext::scanDoneHandler(void *arg)
{
    WiFiContext *self = static_cast<WiFiContext *>(arg);
    self->updateNetList();
}

void WiFiContext::connectToNet(const String &ssid)
{
    String wifi_pass = _settings.get(ssid.c_str(), STR_WIFI_SUBDIR);

    if (wifi_pass.isEmpty())
    {
        _sel_ssid = _main_menu->getCurrItemText();
        showEnterPwdTmpl();
    }
    else
    {
        _wifi.setConnectDoneHandler(connDoneHandler, this);
        if (!_wifi.tryConnectTo(ssid, wifi_pass))
            showToast(STR_FAIL, TOAST_LENGTH_SHORT);
        else
            showToast(STR_CONNECTING, TOAST_LENGTH_SHORT);
    }
}

void WiFiContext::connDoneHandler(void *arg, wl_status_t conn_status)
{
    WiFiContext *self = static_cast<WiFiContext *>(arg);
    if (conn_status != WL_CONNECTED)
        self->showToast(STR_CONNECT_ERR, TOAST_LENGTH_LONG);
    else
    {
        self->showToast(STR_SUCCSESS, TOAST_LENGTH_SHORT);
        self->updateNetList(true);
    }
}
