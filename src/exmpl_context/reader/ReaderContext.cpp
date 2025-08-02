#pragma GCC optimize("O3")
#include "ReaderContext.h"

#include "../WidgetCreator.h"

#include "../resources/string.h"
#include "../resources/color.h"
#include "../resources/const.h"

const char STR_BOOK_DIR_PREF[] = "book_dir";
const char STR_BOOK_NAME_PREF[] = "book_name";
const char STR_READ_POS_PREF[] = "read_pos";
const char STR_BOOK_BRIGHT_PREF[] = "book_bright";

const char ROOT_PATH[] = "/books";
const char BOOK_EXT[] = ".txt";

bool ReaderContext::loop()
{
    return true;
}

void ReaderContext::savePref()
{
    _settings.set(STR_BOOK_DIR_PREF, _dir_name.c_str());
    _settings.set(STR_BOOK_NAME_PREF, _book_name.c_str());
    _settings.set(STR_READ_POS_PREF, String(_read_pos).c_str());
}

//-------------------------------------------------------------------------------------------

void ReaderContext::showContextMenuTmpl()
{
    IWidgetContainer *layout = getLayout();
    layout->disable();

    _books_list_menu->disable();

    WidgetCreator creator;

    _context_menu = new FixedMenu(ID_BOOK_MENU);
    layout->addWidget(_context_menu);
    _context_menu->setBackColor(COLOR_MENU_ITEM);
    _context_menu->setBorderColor(TFT_ORANGE);
    _context_menu->setBorder(true);
    _context_menu->setItemHeight(20);
    _context_menu->setWidth(120);
    _context_menu->setHeight(44);
    _context_menu->setPos(D_WIDTH - _context_menu->getWidth(), D_HEIGHT - _context_menu->getHeight() - TFT_CUTOUT);

    if (_books_list_menu->getCurrItemID() != 0)
    {
        MenuItem *del_item = creator.getMenuItem(ID_ITEM_DEL);
        _context_menu->addItem(del_item);

        Label *upd_lbl = creator.getItemLabel(STR_DELETE, 4, 2);
        del_item->setLbl(upd_lbl);
    }

    _mode = MODE_CONTEXT_MENU;
    layout->enable();
}

void ReaderContext::hideContextMenu()
{
    getLayout()->delWidgetByID(ID_BOOK_MENU);
    _books_list_menu->enable();
    _mode = MODE_BOOK_SEL;
}

void ReaderContext::showBookDirsTmpl()
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    _book_dirs_menu = new FixedMenu(ID_F_MENU);
    layout->addWidget(_book_dirs_menu);
    _book_dirs_menu->setBackColor(COLOR_MENU_ITEM);
    _book_dirs_menu->setWidth(D_WIDTH - SCROLLBAR_WIDTH);
    _book_dirs_menu->setHeight(D_HEIGHT - TFT_CUTOUT * 2);
    _book_dirs_menu->setItemHeight((_book_dirs_menu->getHeight()) / BOOK_DIR_ITEMS_NUM);
    _book_dirs_menu->setPos(0, TFT_CUTOUT);

    _scrollbar = new ScrollBar(ID_SCROLL);
    layout->addWidget(_scrollbar);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(D_HEIGHT - TFT_CUTOUT * 2);
    _scrollbar->setPos(D_WIDTH - SCROLLBAR_WIDTH, TFT_CUTOUT);

    if (!_book_name.isEmpty())
    {
        MenuItem *cont_item = creator.getMenuItem(ID_CONT_ITEM);
        _book_dirs_menu->addItem(cont_item);

        Label *cont_lbl = creator.getItemLabel(STR_CONTINUE, 4, 2);
        cont_item->setLbl(cont_lbl);
    }

    _mode = MODE_BOOK_DIR_SEL;

    setLayout(layout);
}

void ReaderContext::fillBookDirs()
{
    std::vector<MenuItem *> items;
    makeBookDirsItems(items);

    uint16_t size = items.size();

    for (size_t i = 0; i < size; ++i)
        _book_dirs_menu->addItem(items[i]);

    _scrollbar->setValue(0);
    _scrollbar->setMax(_book_dirs_menu->getSize());
}

void ReaderContext::makeBookDirsItems(std::vector<MenuItem *> &items)
{
    WidgetCreator creator;
    items.clear();

    uint16_t books_num = _dirs.size();
    items.reserve(books_num);

    for (uint16_t i = 0, counter = ID_CONT_ITEM; i < books_num; ++i)
    {
        ++counter;
        MenuItem *item = creator.getMenuItem(counter);
        items.push_back(item);

        Label *lbl = new Label(1);
        item->setLbl(lbl);
        lbl->setTickerInFocus(true);

        lbl->setText(_dirs[i].getName());
    }
}

void ReaderContext::showBooksListTmpl()
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    _books_list_menu = creator.getDynamicMenu(ID_D_MENU);
    layout->addWidget(_books_list_menu);
    _books_list_menu->setWidth(D_WIDTH - SCROLLBAR_WIDTH);
    _books_list_menu->setHeight(D_HEIGHT - TFT_CUTOUT * 2);
    _books_list_menu->setItemHeight((_books_list_menu->getHeight() - 2) / BOOKS_ITEMS_NUM);

    _books_list_menu->setOnNextItemsLoadHandler(onNextItemsLoad, this);
    _books_list_menu->setOnPrevItemsLoadHandler(onPrevItemsLoad, this);

    _scrollbar = new ScrollBar(ID_SCROLL);
    layout->addWidget(_scrollbar);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(D_HEIGHT - TFT_CUTOUT);
    _scrollbar->setPos(D_WIDTH - SCROLLBAR_WIDTH, 0);

    _mode = MODE_BOOK_SEL;

    setLayout(layout);
}

void ReaderContext::fillBooks(uint16_t pos)
{
    _books_list_menu->delWidgets();

    uint16_t pl_sz = _books.size();
    if (pl_sz > 0 && pos >= pl_sz)
        --pos;

    std::vector<MenuItem *> items;
    makeBooksItems(items, pos, _books_list_menu->getItemsNumOnScreen());

    uint16_t size = items.size();

    for (size_t i = 0; i < size; ++i)
        _books_list_menu->addItem(items[i]);

    _scrollbar->setMax(pl_sz);
    _scrollbar->setValue(pos);
}

void ReaderContext::makeBooksItems(std::vector<MenuItem *> &items, uint16_t file_pos, uint8_t size)
{
    if (file_pos >= _books.size())
        return;

    uint16_t read_to = file_pos + size;

    if (read_to > _books.size())
        read_to = _books.size();

    WidgetCreator creator;
    items.clear();
    items.reserve(read_to - file_pos);

    for (uint16_t i = file_pos; i < read_to; ++i)
    {
        ++file_pos;

        MenuItem *item = creator.getMenuItem(file_pos);
        items.push_back(item);

        Label *lbl = new Label(1);
        item->setLbl(lbl);
        lbl->setTickerInFocus(true);

        lbl->setText(_books[i].getName());
    }
}

void ReaderContext::showReadTmpl()
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    layout->setBackColor(TFT_BLACK);

    _page = new Label(ID_PAGE_LBL);
    layout->addWidget(_page);
    _page->setMultiline(true);
    _page->setWidth(D_WIDTH - 10);
    _page->setHeight(D_HEIGHT - TFT_CUTOUT * 2);
    _page->setBackColor(TFT_BLACK);
    _page->setTextColor(TFT_WHITE);
    _page->setPos(5, TFT_CUTOUT);

    _progress_lbl = new Label(ID_PROGRESS_LBL);
    layout->addWidget(_progress_lbl);
    _progress_lbl->setText(EMPTY_READ_PROGRESS);
    _progress_lbl->setAlign(IWidget::ALIGN_CENTER);
    _progress_lbl->setGravity(IWidget::GRAVITY_CENTER);
    _progress_lbl->setTextColor(TFT_WHITE);
    _progress_lbl->initWidthToFit();
    _progress_lbl->setPos(getCenterX(_progress_lbl), D_HEIGHT - _progress_lbl->getHeight());

    _mode = MODE_BOOK_READ;

    setLayout(layout);
}

void ReaderContext::showSDErrTmpl()
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    Label *msg_lbl = creator.getStatusMsgLable(ID_MSG_LBL, STR_SD_ERR);
    layout->addWidget(msg_lbl);

    _mode = MODE_SD_UNCONN;

    setLayout(layout);
}

//-------------------------------------------------------------------------------------------

ReaderContext::ReaderContext()
{
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    if (!_sd.isMounted())
    {
        showSDErrTmpl();
        return;
    }

    _dir_name = _settings.get(STR_BOOK_DIR_PREF);
    _book_name = _settings.get(STR_BOOK_NAME_PREF);
    _read_pos = atoi(_settings.get(STR_READ_POS_PREF).c_str());

    showBookDirsTmpl();
    indexDirs();
    fillBookDirs();
}

void ReaderContext::update()
{
    if (_mode == MODE_SD_UNCONN)
    {
        if (_input.isReleased(BtnID::BTN_B))
        {
            _input.lock(BtnID::BTN_B, CLICK_LOCK);
            setCpuFrequencyMhz(240);
            openContextByID(ID_CONTEXT_MENU);
        }

        return;
    }

    if (_input.isReleased(BtnID::BTN_D))
    {
        _input.lock(BtnID::BTN_D, CLICK_LOCK);
        if (_mode == MODE_BOOK_SEL)
            showContextMenuTmpl();
    }
    else if (_input.isPressed(BtnID::BTN_EXIT))
    {
        _input.lock(BtnID::BTN_EXIT, PRESS_LOCK);
        backPressed();
    }
    else if (_input.isHolded(BtnID::BTN_UP))
    {
        _input.lock(BtnID::BTN_UP, HOLD_LOCK);
        up();
    }
    else if (_input.isHolded(BtnID::BTN_DOWN))
    {
        _input.lock(BtnID::BTN_DOWN, HOLD_LOCK);
        down();
    }
    else if (_input.isReleased(BtnID::BTN_RIGHT))
    {
        _input.lock(BtnID::BTN_RIGHT, CLICK_LOCK);
        right();
    }
    else if (_input.isReleased(BtnID::BTN_LEFT))
    {
        _input.lock(BtnID::BTN_LEFT, CLICK_LOCK);
        left();
    }
    else if (_input.isReleased(BtnID::BTN_A))
    {
        _input.lock(BtnID::BTN_A, CLICK_LOCK);
        ok();
    }
    else if (_input.isReleased(BtnID::BTN_B))
    {
        _input.lock(BtnID::BTN_B, CLICK_LOCK);
        back();
    }
}

void ReaderContext::ok()
{
    if (_mode == MODE_BOOK_DIR_SEL)
    {
        uint16_t item_ID = _book_dirs_menu->getCurrItemID();

        if (item_ID == ID_CONT_ITEM)
        {
            indexBooks();
            openBook(true);
        }
        else
        {
            _dir_name = _book_dirs_menu->getCurrItemText();
            _book_pos = 0;
            indexBooks();
            showBooksListTmpl();
            fillBooks(_book_pos);
        }
    }
    else if (_mode == MODE_BOOK_SEL)
    {
        if (_books_list_menu->getSize() > 0)
            openBook(false);
    }
    else if (_mode == MODE_CONTEXT_MENU)
    {
        uint16_t id = _context_menu->getCurrItemID();

        if (id == ID_ITEM_DEL)
        {
            _book_name = _books_list_menu->getCurrItemText();
            if (_book_name.isEmpty())
                return;

            String book_path = getBookPath(_dir_name.c_str(), _book_name.c_str());

            if (_fs.rmFile(book_path.c_str(), true))
            {
                if (_books_list_menu->getCurrItemID() - 2 > -1)
                    _book_pos = _books_list_menu->getCurrItemID() - 2;
                else
                    _book_pos = 0;

                hideContextMenu();
                indexBooks();
                updateBookPos();
                fillBooks(_book_pos);
            }
        }
    }
}

void ReaderContext::updateBookPos()
{
    if (_book_pos > 0)
    {
        if (_book_pos >= _books.size())
            _book_pos = _books.size() - 1;
    }
}

void ReaderContext::indexDirs()
{
    String dirs_path = ROOT_PATH;
    _fs.indexDirs(_dirs, dirs_path.c_str());
}

void ReaderContext::indexBooks()
{
    String books_path = ROOT_PATH;
    books_path += "/";
    books_path += _dir_name;
    _fs.indexFilesExt(_books, books_path.c_str(), BOOK_EXT);
}

void ReaderContext::handleNextItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id)
{
    if (!cur_id)
        return;

    makeBooksItems(items, cur_id, size);
}

void ReaderContext::handlePrevItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id)
{
    if (!cur_id)
        return;

    uint16_t item_pos = cur_id - 1;

    if (!item_pos)
        return;

    if (cur_id > size)
        item_pos = cur_id - size - 1;
    else
    {
        item_pos = 0;
        _scrollbar->setValue(cur_id);
    }

    makeBooksItems(items, item_pos, size);
}

void ReaderContext::onNextItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg)
{
    ReaderContext *self = static_cast<ReaderContext *>(arg);
    self->handleNextItemsLoad(items, size, cur_id);
}

void ReaderContext::onPrevItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg)
{
    ReaderContext *self = static_cast<ReaderContext *>(arg);
    self->handlePrevItemsLoad(items, size, cur_id);
}

//-------------------------------------------------------------------------------------------

void ReaderContext::up()
{
    if (_mode == MODE_BOOK_DIR_SEL)
    {
        _book_dirs_menu->focusUp();
        _scrollbar->scrollUp();
    }
    else if (_mode == MODE_BOOK_SEL)
    {
        _books_list_menu->focusUp();
        _scrollbar->scrollUp();
    }
    else if (_mode == MODE_CONTEXT_MENU)
    {
        _context_menu->focusUp();
    }
}

void ReaderContext::down()
{
    if (_mode == MODE_BOOK_DIR_SEL)
    {
        _book_dirs_menu->focusDown();
        _scrollbar->scrollDown();
    }
    else if (_mode == MODE_BOOK_SEL)
    {
        _books_list_menu->focusDown();
        _scrollbar->scrollDown();
    }
    else if (_mode == MODE_CONTEXT_MENU)
    {
        _context_menu->focusDown();
    }
}

void ReaderContext::left()
{
    if (_mode == MODE_BOOK_READ)
        loadPrevTxt();
}

void ReaderContext::right()
{
    if (_mode == MODE_BOOK_READ)
        loadNextTxt();
}

void ReaderContext::back()
{
    if (_mode == MODE_BOOK_DIR_SEL)
    {
        openContextByID(ID_CONTEXT_MENU);
    }
    else if (_mode == MODE_BOOK_SEL)
    {
        indexDirs();
        showBookDirsTmpl();
        fillBookDirs();
    }
    else if (_mode == MODE_CONTEXT_MENU)
    {
        hideContextMenu();
    }
}

void ReaderContext::backPressed()
{
    if (_mode == MODE_BOOK_READ)
    {
        setCpuFrequencyMhz(240);

        savePref();
        showBooksListTmpl();
        fillBooks(_book_pos);
    }
}

String ReaderContext::getBookPath(const char *dir_name, const char *book_name)
{
    String book_path = ROOT_PATH;
    book_path += "/";
    book_path += dir_name;
    book_path += "/";
    book_path += book_name;
    return book_path;
}

void ReaderContext::openBook(bool contn)
{
    if (contn)
    {
        if ((_dir_name.isEmpty() || _book_name.isEmpty()))
            return;
    }
    else
    {
        _book_name = _books_list_menu->getCurrItemText();
        _book_pos = _books_list_menu->getCurrItemID() - 1;
        _read_pos = 0;
    }

    String book_path = getBookPath(_dir_name.c_str(), _book_name.c_str());

    _book_size = _fs.getFileSize(book_path.c_str());

    if (containCyrillic(_dir_name.c_str(), _book_name.c_str()))
        _num_char_to_read = KIR_NUM_NYTES_TO_READ;
    else
        _num_char_to_read = LAT_NUM_BYTES_TO_READ;

    if (_book_size > 0)
    {
        showReadTmpl();
        loadNextTxt();
        updateReadProgress();
    }
}

void ReaderContext::updateReadProgress()
{
    String prog_txt;
    prog_txt += _read_pos / _num_char_to_read;
    prog_txt += "/";
    prog_txt += _book_size / _num_char_to_read;

    _progress_lbl->setText(prog_txt);
    _progress_lbl->updateWidthToFit();
}

void ReaderContext::loadNextTxt()
{
    setCpuFrequencyMhz(240);

    String txt;
    readText(txt, _dir_name.c_str(), _book_name.c_str(), _num_char_to_read, _read_pos);

    if (!txt.isEmpty())
    {
        _read_pos += txt.length();

        _page->setText(txt);
        updateReadProgress();
    }

    setCpuFrequencyMhz(80);
}

void ReaderContext::loadPrevTxt()
{
    if (_read_pos == 0)
        return;

    setCpuFrequencyMhz(240);

    if (_read_pos > _num_char_to_read)
        _read_pos -= _num_char_to_read;
    else
        _read_pos = 0;

    String txt;
    readText(txt, _dir_name.c_str(), _book_name.c_str(), _num_char_to_read, _read_pos);

    if (!txt.isEmpty())
    {
        _page->setText(txt);
        updateReadProgress();
    }

    setCpuFrequencyMhz(80);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------

bool ReaderContext::isCyrillic(char ch)
{
    unsigned char uc = static_cast<unsigned char>(ch);
    return uc >= 0x80;
}

bool ReaderContext::containCyrillic(const char *dir_name, const char *book_name)
{
    String path = getBookPath(dir_name, book_name);

    const uint8_t ARR_SIZE = 160;
    char ch_arr[ARR_SIZE];

    size_t read_bytes = _fs.readFile(path.c_str(), ch_arr, ARR_SIZE);

    for (size_t i{0}; i < read_bytes; ++i)
    {
        if (isCyrillic(ch_arr[i]))
            return true;
    }

    return false;
}

bool ReaderContext::readText(String &out_str, const char *dir_name, const char *book_name, size_t len, size_t pos)
{
    char *buffer;
    if (psramInit())
        buffer = static_cast<char *>(ps_malloc(len + 1));
    else
        buffer = static_cast<char *>(malloc(len + 1));

    if (!buffer)
    {
        log_e("alloc error: %zu b", len + 1);
        out_str = "";
        return false;
    }

    String book_path = getBookPath(dir_name, book_name);

    size_t bytes_read = _fs.readFile(book_path.c_str(), buffer, len, pos);
    buffer[bytes_read] = '\0';

    bool is_oef = false;

    if (bytes_read != len)
        is_oef = true;

    if (bytes_read > 2 && (buffer[bytes_read - 1] & 0x80) != 0)
    {
        size_t correct_char_pos{0};

        for (size_t i{bytes_read - 2}; i > 0; --i)
        {
            if ((buffer[i] & 0x80) == 0)
            {
                correct_char_pos = i;
                break;
            }
        }

        if (correct_char_pos > 0)
        {
            char *temp_buff = static_cast<char *>(malloc(correct_char_pos + 2));
            if (temp_buff)
            {
                memcpy(temp_buff, buffer, correct_char_pos + 1);
                temp_buff[correct_char_pos + 1] = '\0';
                out_str = String(temp_buff, correct_char_pos);
                free(temp_buff);
            }
            else
                log_e("Помилка створення temp буферу на %zu байт", len);
        }
    }
    else
    {
        out_str = String(buffer, bytes_read);
    }

    free(buffer);
    return !is_oef;
}
