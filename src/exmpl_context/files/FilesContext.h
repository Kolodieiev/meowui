#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include "meow/ui/context/IContext.h"
#include "meow/manager/files/FileManager.h"
#include "meow/lib/server/file_server/FileServer.h"
//
#include "meow/plugin/lua/context/LuaContext.h"
#include "meow/ui/widget/notification/Notification.h"
//
#include "meow/ui/widget/scrollbar/ScrollBar.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/ui/widget/menu/DynamicMenu.h"
#include "meow/ui/widget/keyboard/Keyboard.h"
#include "meow/ui/widget/progress/ProgressBar.h"
#include "meow/ui/widget/image/Image.h"
#include "meow/ui/widget/text/TextBox.h"

using namespace meow;

class FilesContext : public IContext
{
public:
    FilesContext();
    virtual ~FilesContext();

protected:
    virtual bool loop() override;
    virtual void update() override;

private:
    const uint8_t MENU_ITEMS_NUM{7};

    enum Widget_ID : uint8_t
    {
        ID_SCROLLBAR = 1,
        ID_DYNAMIC_MENU,
        ID_CNTXT_MENU,
        ID_KEYBOARD,
        ID_DIALOG_TXT,
        ID_MSG_LBL,
        ID_PROGRESS,
        ID_QR_IMG,
        ID_QR_LBL,
        ID_SIZE_TITLE_LBL,
        ID_SIZE_LBL,
        ID_FILE_POS_LBL,
    };

    enum Item_ID : uint8_t
    {
        ID_ITEM_UPDATE = 1,
        ID_ITEM_PASTE,
        ID_ITEM_MOVE,
        ID_ITEM_COPY,
        ID_ITEM_REMOVE,
        ID_ITEM_NEW_DIR,
        ID_ITEM_EXECUTE,
        ID_ITEM_RENAME,
        ID_ITEM_IMPORT,
        ID_ITEM_EXPORT,
    };

    enum Mode : uint8_t
    {
        MODE_NAVIGATION = 0,
        MODE_LUA,
        MODE_NOTIFICATION,
        MODE_COPYING,
        MODE_REMOVING,
        MODE_MOVING,
        MODE_CONTEXT_MENU,
        MODE_NEW_DIR_DIALOG,
        MODE_RENAME_DIALOG,
        MODE_SD_UNCONN,
        MODE_CANCELING,
        MODE_FILE_SERVER
    };
    //
    Label *_msg_lbl;
    const uint16_t UPD_TRACK_INF_INTERVAL{1000};
    unsigned long _upd_msg_time{0};
    uint8_t _upd_counter{0};
    //
    Mode _mode{MODE_NAVIGATION};
    //
    FileServer _server;
    Image *_qr_img;
    uint16_t *_qr_img_buff = nullptr;
    uint16_t _qr_width = 0;
    uint8_t _display_brightness = 125;
    bool _is_back_eabled = true;
    //
    FixedMenu *_context_menu;
    ScrollBar *_scrollbar;
    ProgressBar *_task_progress;
    DynamicMenu *_files_list;
    Image *_dir_img;
    Image *_lua_img;
    //
    Keyboard *_keyboard;
    TextBox *_dialog_txt;
    bool _dialog_success_res{false};
    String _old_name;
    //
    bool _is_dir{false};
    bool _has_moving_file{false};
    bool _has_copying_file{false};
    String _path_from;
    String _name_from;
    String _copy_to_path;
    //
    Label *_file_size_lbl;
    Label *_file_pos_lbl;
    //
    std::vector<String> _breadcrumbs;
    void makePathFromBreadcrumbs(String &out_str);
    //
    std::vector<FileInfo> _files;
    //
    LuaContext *_lua_context{nullptr};
    Notification *_notification;
    //
    void showFilesTmpl();
    void showCopyingTmpl();
    void showRemovingTmpl();
    void showCancelingTmpl();
    void showSDErrTmpl();
    void showServerTmpl();
    //
    void showContextMenu();
    void hideContextMenu();
    //
    void keyboardClickHandler();
    void showDialog(Mode mode);
    void hideDialog();
    //
    void prepareFileMoving();
    void prepareFileCopying();
    void pasteFile();
    void removeFile();
    //
    void ok();
    void back();
    void up();
    void down();
    void updateFileInfo();
    //
    void openNextLevel();
    void openPrevlevel();
    //
    void indexCurDir();
    void fillFilesTmpl();
    void saveDialogResult();

    void makeMenuFilesItems(std::vector<MenuItem *> &items, uint16_t file_pos, uint8_t size);
    //
    void startFileServer(FileServer::ServerMode mode);
    void stopFileServer();

    void taskDoneHandler(bool result);
    static void taskDone(bool result, void *arg);
    //
    void handleNextItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id);
    static void onNextItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg);
    //
    void handlePrevItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id);
    static void onPrevItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg);
    //
    void showResultToast(bool result);
    //
    void createNotificationObj();
    //
    void executeScript();
};