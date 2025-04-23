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
#include "./res/book.h"

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
    // Музика
    MenuItem *mp3_item = creator.getMenuItem(ID_CONTEXT_MP3);
    _menu->addItem(mp3_item);

    Image *mp3_img = new Image(1);
    mp3_item->setIco(mp3_img);
    mp3_img->setTransparentColor(TFT_TRANSPARENT);
    mp3_img->init(ICO_WH, ICO_WH);
    mp3_img->setSrc(HEADPHONES_IMG);

    Label *mp3_lbl = creator.getItemLabel(STR_MUSIC_ITEM, 4, 2);
    mp3_item->setLbl(mp3_lbl);

    // Читалка
    MenuItem *read_item = creator.getMenuItem(ID_CONTEXT_READER);
    _menu->addItem(read_item);

    Image *read_img = new Image(1);
    read_item->setIco(read_img);
    read_img->setTransparentColor(TFT_TRANSPARENT);
    read_img->init(ICO_WH, ICO_WH);
    read_img->setSrc(BOOK_IMG);

    Label *read_lbl = creator.getItemLabel(STR_READER_ITEM, 4, 2);
    read_item->setLbl(read_lbl);

    // Ігри
    MenuItem *game_item = creator.getMenuItem(ID_CONTEXT_GAMES);
    _menu->addItem(game_item);

    Image *game_img = new Image(1);
    game_item->setIco(game_img);
    game_img->setTransparentColor(TFT_TRANSPARENT);
    game_img->init(ICO_WH, ICO_WH);
    game_img->setSrc(JOYSTICK_IMG);

    Label *game_lbl = creator.getItemLabel(STR_GAME_ITEM, 4, 2);
    game_item->setLbl(game_lbl);

    // Файли
    MenuItem *files_item = creator.getMenuItem(ID_CONTEXT_FILES);
    _menu->addItem(files_item);

    Image *files_img = new Image(1);
    files_item->setIco(files_img);
    files_img->setTransparentColor(TFT_TRANSPARENT);
    files_img->init(ICO_WH, ICO_WH);
    files_img->setSrc(SD_IMG);

    Label *files_lbl = creator.getItemLabel(STR_FILES_ITEM, 4, 2);
    files_item->setLbl(files_lbl);

    // Налаштування
    MenuItem *pref_item = creator.getMenuItem(ID_CONTEXT_PREF_SEL);
    _menu->addItem(pref_item);

    Image *pref_img = new Image(1);
    pref_item->setIco(pref_img);
    pref_img->setTransparentColor(TFT_TRANSPARENT);
    pref_img->init(ICO_WH, ICO_WH);
    pref_img->setSrc(SETTINGS_IMG);

    Label *pref_lbl = creator.getItemLabel(STR_PREFERENCES_ITEM, 4, 2);
    pref_item->setLbl(pref_lbl);

    // Прошика
    MenuItem *firm_item = creator.getMenuItem(ID_CONTEXT_FIRMWARE);
    _menu->addItem(firm_item);

    Image *firm_img = new Image(1);
    firm_item->setIco(firm_img);
    firm_img->setTransparentColor(TFT_TRANSPARENT);
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

    _menu->setCurrentFocusPos(_last_sel_item_pos);
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
    if (_input.isHolded(KeyID::KEY_C))
    {
        _input.lock(KeyID::KEY_C, HOLD_LOCK);
        up();
    }
    else if (_input.isHolded(KeyID::KEY_B))
    {
        _input.lock(KeyID::KEY_B, HOLD_LOCK);
        down();
    }
    else if (_input.isReleased(KeyID::KEY_A))
    {
        _input.lock(KeyID::KEY_A, CLICK_LOCK);
        ok();
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
    log_e("Це лише приклад прошивки!");
    // uint16_t id = _menu->getCurrentItemID();
    // _last_sel_item_pos = _menu->getCurrentFocusPos();
    // openContextByID((ContextID)id);
}
