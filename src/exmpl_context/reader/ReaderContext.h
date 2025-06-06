#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>

#include "meow/ui/context/IContext.h"
#include "meow/manager/settings/SettingsManager.h"
#include "meow/ui/widget/scrollbar/ScrollBar.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/ui/widget/menu/DynamicMenu.h"

using namespace meow;

class ReaderContext : public IContext
{

public:
    ReaderContext();
    virtual ~ReaderContext() {}

protected:
    virtual bool loop() override;
    virtual void update() override;

private:
    enum Widget_ID : uint8_t
    {
        ID_F_MENU = 1,
        ID_D_MENU,
        ID_SCROLL,
        ID_BOOK_MENU,
        ID_PROGRESS,
        ID_TIME_LBL,
        ID_UPD_LBL,
        ID_PAGE_LBL,
        ID_PROGRESS_LBL,
        ID_MSG_LBL
    };

    enum Mode : uint8_t
    {
        MODE_BOOK_DIR_SEL = 0,
        MODE_BOOK_SEL,
        MODE_BOOK_READ,
        MODE_CONTEXT_MENU,
        MODE_SD_UNCONN
    };

    enum BlMenuItemsID : uint8_t
    {
        ID_ITEM_DEL = 1,
        ID_CONT_ITEM,
    };
    //
    Mode _mode{MODE_BOOK_DIR_SEL};
    unsigned long _upd_msg_time{0};
    //
    SettingsManager _settings;

    const uint8_t BOOK_DIR_ITEMS_NUM{7};
    const uint8_t BOOKS_ITEMS_NUM{7};
    const uint16_t LAT_NUM_BYTES_TO_READ{320};
    const uint16_t KIR_NUM_NYTES_TO_READ{574};
    uint16_t _num_char_to_read{0};
    //
    Label *_progress_lbl;
    //
    Label *_page;
    ScrollBar *_scrollbar;
    FixedMenu *_context_menu;
    FixedMenu *_book_dirs_menu;
    DynamicMenu *_books_list_menu;
    //
    String _dir_name;
    String _book_name;
    uint16_t _book_pos{0};
    size_t _book_size{0};
    size_t _read_pos{0};

    //
    std::vector<FileInfo> _dirs;
    std::vector<FileInfo> _books;
    //
    void savePref();
    //
    void ok();
    void up();
    void down();
    void left();
    void right();
    void back();
    void backPressed();

    //
    void showBookDirsTmpl();
    void fillBookDirs();
    void makeBookDirsItems(std::vector<MenuItem *> &items);
    //
    void showBooksListTmpl();
    void fillBooks(uint16_t pos = 0);
    void makeBooksItems(std::vector<MenuItem *> &items, uint16_t file_pos, uint8_t size);
    //
    void showContextMenuTmpl();
    void hideContextMenu();
    //
    void openBook(bool contn = false);
    void loadNextTxt();
    void loadPrevTxt();
    //
    void showSDErrTmpl();
    void showReadTmpl();
    //
    void updateReadProgress();
    void updateBookPos();
    //
    void indexDirs();
    void indexBooks();
    //
    void handleNextItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id);
    static void onNextItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg);
    //
    void handlePrevItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id);
    static void onPrevItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg);

    //

    String getBookPath(const char *dir_name, const char *book_name);
    bool isCyrillic(char ch);
    bool containCyrillic(const char *dir_name, const char *book_name);
    bool readText(String &out_str, const char *dir_name, const char *book_name, size_t len, size_t pos);
};