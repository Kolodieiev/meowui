#include "MenuContext.h"

#include "../resources/color.h"
#include "../resources/string.h"
#include "../resources/const.h"
//
#include "./res/headphones.h"
#include "./res/joystick.h"
#include "./res/sd.h"
#include "./res/settings.h"
#include "./res/chip.h"
#include "./res/calc.h"
#include "./res/book.h"
#include "./res/lua_ico.h"
#include "./res/wifi_ico.h"

#include "../WidgetCreator.h"
#include "meow/ui/widget/layout/EmptyLayout.h"
#include "meow/ui/widget/menu/item/MenuItem.h"

#define ICO_WH 35

uint8_t MenuContext::_last_sel_item_pos;

MenuContext::MenuContext()
{
    WidgetCreator creator;
    //
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);
    //
    _menu = new FixedMenu(ID_MENU);
    layout->addWidget(_menu);
    _menu->setBackColor(COLOR_MAIN_BACK);
    _menu->setWidth(_display.width() - SCROLLBAR_WIDTH - DISPLAY_PADDING * 2);
    _menu->setHeight(_display.height() - TFT_CUTOUT * 2);
    _menu->setItemHeight((_menu->getHeight() - 2) / 4);
    _menu->setPos(DISPLAY_PADDING, TFT_CUTOUT);
    //
    _scrollbar = new ScrollBar(ID_SCROLLBAR);
    layout->addWidget(_scrollbar);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(_menu->getHeight());
    _scrollbar->setPos(_menu->getWidth() + _menu->getXPos(), _menu->getYPos());
    _scrollbar->setBackColor(COLOR_MAIN_BACK);

    // Файли
    MenuItem *files_item = creator.getMenuItem(ID_CONTEXT_FILES);
    _menu->addItem(files_item);

    Image *files_img = new Image(1);
    files_item->setImg(files_img);
    files_img->setTranspColor(TFT_TRANSPARENT);
    files_img->init(ICO_WH, ICO_WH);
    files_img->setSrc(SD_IMG);

    Label *files_lbl = creator.getItemLabel(STR_FILES_ITEM, 4, 2);
    files_item->setLbl(files_lbl);

    // Ігри
    MenuItem *game_item = creator.getMenuItem(ID_CONTEXT_GAMES);
    _menu->addItem(game_item);

    Image *game_img = new Image(1);
    game_item->setImg(game_img);
    game_img->setTranspColor(TFT_TRANSPARENT);
    game_img->init(ICO_WH, ICO_WH);
    game_img->setSrc(JOYSTICK_IMG);

    Label *game_lbl = creator.getItemLabel(STR_GAME_ITEM, 4, 2);
    game_item->setLbl(game_lbl);

    // WiFi
    MenuItem *wifi_item = creator.getMenuItem(ID_CONTEXT_WIFI);
    _menu->addItem(wifi_item);

    Image *wifi_img = new Image(1);
    wifi_item->setImg(wifi_img);
    wifi_img->setTranspColor(TFT_TRANSPARENT);
    wifi_img->init(ICO_WH, ICO_WH);
    wifi_img->setSrc(WIFI_IMG);

    Label *wifi_lbl = creator.getItemLabel(STR_WIFI_ITEM, 4, 2);
    wifi_item->setLbl(wifi_lbl);

    // Музика
    MenuItem *mp3_item = creator.getMenuItem(ID_CONTEXT_MP3);
    _menu->addItem(mp3_item);

    Image *mp3_img = new Image(1);
    mp3_item->setImg(mp3_img);
    mp3_img->setTranspColor(TFT_TRANSPARENT);
    mp3_img->init(ICO_WH, ICO_WH);
    mp3_img->setSrc(HEADPHONES_IMG);

    Label *mp3_lbl = creator.getItemLabel(STR_MUSIC_ITEM, 4, 2);
    mp3_item->setLbl(mp3_lbl);

    // Читалка
    MenuItem *read_item = creator.getMenuItem(ID_CONTEXT_READER);
    _menu->addItem(read_item);

    Image *read_img = new Image(1);
    read_item->setImg(read_img);
    read_img->setTranspColor(TFT_TRANSPARENT);
    read_img->init(ICO_WH, ICO_WH);
    read_img->setSrc(BOOK_IMG);

    Label *read_lbl = creator.getItemLabel(STR_READER_ITEM, 4, 2);
    read_item->setLbl(read_lbl);

    // Налаштування
    MenuItem *pref_item = creator.getMenuItem(ID_CONTEXT_PREF_SEL);
    _menu->addItem(pref_item);

    Image *pref_img = new Image(1);
    pref_item->setImg(pref_img);
    pref_img->setTranspColor(TFT_TRANSPARENT);
    pref_img->init(ICO_WH, ICO_WH);
    pref_img->setSrc(SETTINGS_IMG);

    Label *pref_lbl = creator.getItemLabel(STR_PREFERENCES_ITEM, 4, 2);
    pref_item->setLbl(pref_lbl);

    // Прошика
    MenuItem *firm_item = creator.getMenuItem(ID_CONTEXT_FIRMWARE);
    _menu->addItem(firm_item);

    Image *firm_img = new Image(1);
    firm_item->setImg(firm_img);
    firm_img->setTranspColor(TFT_TRANSPARENT);
    firm_img->init(ICO_WH, ICO_WH);
    firm_img->setSrc(CHIP_IMG);

    Label *firm_lbl = creator.getItemLabel(STR_FIRMWARE_ITEM, 4, 2);
    firm_item->setLbl(firm_lbl);
    //

    _bin.reserve(_menu->getSize());
    _bin.push_back(mp3_img);
    _bin.push_back(read_img);
    _bin.push_back(game_img);
    _bin.push_back(files_img);
    _bin.push_back(pref_img);
    _bin.push_back(firm_img);

    _scrollbar->setMax(_menu->getSize());

    _menu->setCurrFocusPos(_last_sel_item_pos);
    _scrollbar->setValue(_last_sel_item_pos);
}

MenuContext::~MenuContext()
{
    for (auto b_it = _bin.begin(), e_it = _bin.end(); b_it != e_it; ++b_it)
        delete *b_it;
}

bool MenuContext::loop()
{
    return true;
}

void MenuContext::update()
{
    if (_input.isHolded(BtnID::BTN_UP))
    {
        _input.lock(BtnID::BTN_UP, HOLD_LOCK);
        up();
    }
    else if (_input.isHolded(BtnID::BTN_DOWN))
    {
        _input.lock(BtnID::BTN_DOWN, HOLD_LOCK);
        down();
    }
    else if (_input.isReleased(BtnID::BTN_A))
    {
        _input.lock(BtnID::BTN_A, CLICK_LOCK);
        ok();
    }
    else if (_input.isReleased(BtnID::BTN_B))
    {
        _input.lock(BtnID::BTN_B, CLICK_LOCK);
        _last_sel_item_pos = 0;
        openContextByID(ID_CONTEXT_HOME);
    }
}

void MenuContext::up()
{
    _menu->focusUp();
    _scrollbar->scrollUp();
}

void MenuContext::down()
{
    _menu->focusDown();
    _scrollbar->scrollDown();
}

void MenuContext::ok()
{
    uint16_t id = _menu->getCurrItemID();
    _last_sel_item_pos = _menu->getCurrFocusPos();
    openContextByID((ContextID)id);
}
