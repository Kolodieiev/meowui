#pragma GCC optimize("O3")
#include "FilesContext.h"

#include "meow/lib/qr/QR_Gen.h"
#include "meow/manager/settings/SettingsManager.h"
//
#include "meow/ui/widget/progress/ProgressBar.h"
#include "meow/ui/widget/menu/item/MenuItem.h"

#include "../WidgetCreator.h"

#include "../resources/color.h"
#include "../resources/string.h"
#include "../resources/const.h"
#include "./res/folder.h"
#include "./res/lua.h"

const char STR_SIZE[] = "File size:";
const char STR_LUA_EXT[] = ".lua";

bool FilesContext::loop()
{
    if (_mode == MODE_LUA)
    {
        if (_lua_context->isReleased())
        {
            String msg = _lua_context->getMsg();
            if (!msg.equals(""))
            {
                _mode = MODE_NOTIFICATION;
                _notification->setMsgText(msg);
                showNotification(_notification);
            }
            else
            {
                _mode = MODE_NAVIGATION;
                getLayout()->forcedDraw();
            }

            delete _lua_context;
            _lua_context = nullptr;
        }
        else
        {
            _lua_context->tick();
            return false;
        }
    }

    return true;
}

FilesContext::FilesContext()
{
    _dir_img = new Image(1);
    _dir_img->setTranspColor(TFT_TRANSPARENT);
    _dir_img->init(16, 16);
    _dir_img->setSrc(FOLDER_IMG);
    _dir_img->setTransparency(true);

    _lua_img = new Image(1);
    _lua_img->setTranspColor(TFT_TRANSPARENT);
    _lua_img->init(16, 16);
    _lua_img->setSrc(LUA_IMG);
    _lua_img->setTransparency(true);

    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    if (!_fs.isMounted())
    {
        showSDErrTmpl();
        return;
    }

    createNotificationObj();

    _fs.setTaskDoneHandler(taskDone, this);

    showFilesTmpl();
    indexCurDir();
    fillFilesTmpl();
}

FilesContext::~FilesContext()
{
    delete _dir_img;
    delete _lua_img;
    delete _lua_context;
    delete _notification;
}

//-------------------------------------------------------------------------------------------

void FilesContext::showSDErrTmpl()
{
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();

    _msg_lbl = creator.getStatusMsgLable(ID_MSG_LBL, STR_SD_ERR);
    layout->addWidget(_msg_lbl);

    _mode = MODE_SD_UNCONN;

    setLayout(layout);
}

void FilesContext::showServerTmpl()
{
    WidgetCreator creator;

    EmptyLayout *layout = creator.getEmptyLayout();

    String header_str;

    if (_server.getServerMode() == FileServer::SERVER_MODE_SEND)
        header_str = STR_EXPORT;
    else
        header_str = STR_IMPORT;

    header_str += ": ";
    header_str += _server.getAddress().c_str();

    _msg_lbl = creator.getStatusMsgLable(ID_MSG_LBL, header_str.c_str());
    layout->addWidget(_msg_lbl);
    _msg_lbl->setHeight(_msg_lbl->getCharHgt() + 4);

    if (_qr_img_buff)
    {
        _qr_img = new Image(ID_QR_IMG);
        layout->addWidget(_qr_img);
        _qr_img->init(_qr_width, _qr_width);
        _qr_img->setSrc(_qr_img_buff);
        _qr_img->setPos((D_WIDTH - _qr_width) / 2, (D_HEIGHT - _qr_width) / 2);
    }

    _mode = MODE_FILE_SERVER;
    setLayout(layout);
}

void FilesContext::showCopyingTmpl()
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    _msg_lbl = creator.getStatusMsgLable(ID_MSG_LBL, STR_COPYING, 2);
    layout->addWidget(_msg_lbl);
    _msg_lbl->setHeight(32);
    _msg_lbl->setPos(0, D_HEIGHT / 2 - _msg_lbl->getHeight() - 2);

    _task_progress = new ProgressBar(ID_PROGRESS);
    layout->addWidget(_task_progress);
    _task_progress->setBackColor(TFT_BLACK);
    _task_progress->setProgressColor(TFT_ORANGE);
    _task_progress->setBorderColor(TFT_WHITE);
    _task_progress->setMax(100);
    _task_progress->setWidth(D_WIDTH - 5 * 8);
    _task_progress->setHeight(20);
    _task_progress->setProgress(0);
    _task_progress->setPos((D_WIDTH - _task_progress->getWidth()) / 2, D_HEIGHT / 2 + 2);

    _mode = MODE_COPYING;

    setLayout(layout);
}

void FilesContext::showRemovingTmpl()
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    _msg_lbl = creator.getStatusMsgLable(ID_MSG_LBL, STR_REMOVING, 2);
    layout->addWidget(_msg_lbl);

    _mode = MODE_REMOVING;

    setLayout(layout);
}

void FilesContext::showCancelingTmpl()
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    _msg_lbl = creator.getStatusMsgLable(ID_MSG_LBL, STR_CANCELING, 2);
    layout->addWidget(_msg_lbl);

    _mode = MODE_CANCELING;
    setLayout(layout);
}

void FilesContext::showFilesTmpl()
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    layout->setBackColor(TFT_BLACK);

    _files_list = creator.getDynamicMenu(ID_DYNAMIC_MENU);
    layout->addWidget(_files_list);
    _files_list->setItemHeight(_files_list->getHeight() / MENU_ITEMS_NUM);
    _files_list->setWidth(D_WIDTH - SCROLLBAR_WIDTH);
    _files_list->setPos(0, TFT_CUTOUT);

    _files_list->setOnNextItemsLoadHandler(onNextItemsLoad, this);
    _files_list->setOnPrevItemsLoadHandler(onPrevItemsLoad, this);
    _scrollbar = new ScrollBar(ID_SCROLLBAR);
    layout->addWidget(_scrollbar);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(D_HEIGHT - TFT_CUTOUT * 2);
    _scrollbar->setPos(D_WIDTH - SCROLLBAR_WIDTH, TFT_CUTOUT);
    _scrollbar->setBackColor(COLOR_MAIN_BACK);

    Label *size_title_lbl = new Label(ID_SIZE_TITLE_LBL);
    layout->addWidget(size_title_lbl);
    size_title_lbl->setText(STR_SIZE);
    size_title_lbl->setFontID(4);
    size_title_lbl->setTextColor(TFT_WHITE);
    size_title_lbl->initWidthToFit();
    size_title_lbl->setPos(TFT_CUTOUT, D_HEIGHT - size_title_lbl->getHeight() * 2 - 4);

    _file_size_lbl = new Label(ID_SIZE_LBL);
    layout->addWidget(_file_size_lbl);
    _file_size_lbl->setText("0");
    _file_size_lbl->setFontID(4);
    _file_size_lbl->setTextColor(TFT_WHITE);
    _file_size_lbl->initWidthToFit();
    _file_size_lbl->setPos(size_title_lbl->getXPos() + size_title_lbl->getWidth() + 2, size_title_lbl->getYPos());

    _file_pos_lbl = new Label(ID_FILE_POS_LBL);
    layout->addWidget(_file_pos_lbl);
    _file_pos_lbl->setText("[0/0]");
    _file_pos_lbl->setFontID(4);
    _file_pos_lbl->setTextColor(TFT_WHITE);
    _file_pos_lbl->initWidthToFit();
    _file_pos_lbl->setPos(TFT_CUTOUT, D_HEIGHT - _file_pos_lbl->getHeight() - 2);

    _mode = MODE_NAVIGATION;

    setLayout(layout);
}

//-------------------------------------------------------------------------------------------

void FilesContext::makePathFromBreadcrumbs(String &out_str)
{
    out_str = "";
    for (uint8_t i{0}; i < _breadcrumbs.size(); ++i)
        out_str += _breadcrumbs[i];
}

void FilesContext::showContextMenu()
{
    _mode = MODE_CONTEXT_MENU;
    _files_list->disable();

    WidgetCreator creator;

    _context_menu = new FixedMenu(ID_CNTXT_MENU);
    getLayout()->addWidget(_context_menu);
    _context_menu->setItemHeight(18);
    _context_menu->setWidth((float)D_WIDTH / 2.2);
    _context_menu->setBackColor(TFT_BLACK);
    _context_menu->setBorder(true);
    _context_menu->setBorderColor(TFT_ORANGE);
    _context_menu->setLoopState(true);

    if (_has_moving_file || _has_copying_file)
    {
        // Якщо є файл для переміщення додати відповідний пункт меню
        MenuItem *paste_item = creator.getMenuItem(ID_ITEM_PASTE);
        _context_menu->addItem(paste_item);

        Label *paste_lbl = creator.getItemLabel(STR_PASTE, 2);
        paste_item->setLbl(paste_lbl);
        paste_lbl->setTextOffset(1);
    }

    // новий каталог
    MenuItem *new_dir_item = creator.getMenuItem(ID_ITEM_NEW_DIR);
    _context_menu->addItem(new_dir_item);

    Label *new_dir_lbl = creator.getItemLabel(STR_NEW_DIR, 2);
    new_dir_item->setLbl(new_dir_lbl);
    new_dir_lbl->setTextOffset(1);

    // контекст для вибраного файлу
    uint16_t id = _files_list->getCurrItemID();

    if (id > 0)
    {
        // перейменувати
        MenuItem *rename_item = creator.getMenuItem(ID_ITEM_RENAME);
        _context_menu->addItem(rename_item);

        Label *rename_lbl = creator.getItemLabel(STR_RENAME, 2);
        rename_item->setLbl(rename_lbl);
        rename_lbl->setTextOffset(1);

        // копіювати
        if (_files.size() >= id && !_files[id - 1].isDir())
        {
            MenuItem *copy_item = creator.getMenuItem(ID_ITEM_COPY);
            _context_menu->addItem(copy_item);

            Label *copy_lbl = creator.getItemLabel(STR_COPY, 2);
            copy_item->setLbl(copy_lbl);
            copy_lbl->setTextOffset(1);
        }

        // перемістити
        MenuItem *move_item = creator.getMenuItem(ID_ITEM_MOVE);
        _context_menu->addItem(move_item);

        Label *move_lbl = creator.getItemLabel(STR_MOVE, 2);
        move_item->setLbl(move_lbl);
        move_lbl->setTextOffset(1);

        // видалити
        MenuItem *delete_item = creator.getMenuItem(ID_ITEM_REMOVE);
        _context_menu->addItem(delete_item);

        Label *delete_lbl = creator.getItemLabel(STR_DELETE, 2);
        delete_item->setLbl(delete_lbl);
        delete_lbl->setTextOffset(1);
    }

    // імпорт
    MenuItem *import_item = creator.getMenuItem(ID_ITEM_IMPORT);
    _context_menu->addItem(import_item);

    Label *import_lbl = creator.getItemLabel(STR_IMPORT, 2);
    import_item->setLbl(import_lbl);
    import_lbl->setTextOffset(1);

    // експорт
    MenuItem *export_item = creator.getMenuItem(ID_ITEM_EXPORT);
    _context_menu->addItem(export_item);

    Label *export_lbl = creator.getItemLabel(STR_EXPORT, 2);
    export_item->setLbl(export_lbl);
    export_lbl->setTextOffset(1);

    // оновити
    MenuItem *upd_item = creator.getMenuItem(ID_ITEM_UPDATE);
    _context_menu->addItem(upd_item);

    Label *upd_lbl = creator.getItemLabel(STR_UPDATE, 2);
    upd_item->setLbl(upd_lbl);
    upd_lbl->setTextOffset(1);

    // Виконати
    if (!_files[id - 1].isDir() && _files[id - 1].nameEndsWith(STR_LUA_EXT))
    {
        MenuItem *exec_item = creator.getMenuItem(ID_ITEM_EXECUTE);
        _context_menu->addItem(exec_item);

        Label *exec_lbl = creator.getItemLabel(STR_EXECUTE, 2);
        exec_item->setLbl(exec_lbl);
        exec_lbl->setTextOffset(1);
    }
    //
    _context_menu->setHeight(_context_menu->getSize() * _context_menu->getItemHeight() + 4);
    _context_menu->setPos(D_WIDTH - _context_menu->getWidth() - 1,
                          D_HEIGHT - TFT_CUTOUT - _context_menu->getHeight() - 2);
}

void FilesContext::hideContextMenu()
{
    getLayout()->delWidgetByID(ID_CNTXT_MENU);
    _mode = MODE_NAVIGATION;
    _files_list->enable();
}

//-------------------------------------------------------------------------------------------

void FilesContext::showDialog(Mode mode)
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    _dialog_txt = new TextBox(ID_DIALOG_TXT);
    _dialog_txt->setTextOffset(5);
    _dialog_txt->setWidth(D_WIDTH - 10);
    _dialog_txt->setHeight(40);
    _dialog_txt->setBackColor(TFT_WHITE);
    _dialog_txt->setTextColor(TFT_BLACK);
    _dialog_txt->setFontID(2);
    _dialog_txt->setTextSize(2);
    _dialog_txt->setPos(5, TFT_CUTOUT);
    _dialog_txt->setCornerRadius(3);

    _mode = mode;
    if (_mode == MODE_RENAME_DIALOG)
    {
        _old_name = _files_list->getCurrItemText();
        _dialog_txt->setText(_old_name);
    }

    _keyboard = creator.getStandardEnKeyboard(ID_KEYBOARD);

    layout->setBackColor(TFT_BLACK);
    layout->addWidget(_dialog_txt);
    layout->addWidget(_keyboard);

    setLayout(layout);
}

void FilesContext::hideDialog()
{
    showFilesTmpl();
    if (_dialog_success_res)
        indexCurDir();
    fillFilesTmpl();
}

void FilesContext::saveDialogResult()
{
    if (_mode == MODE_NEW_DIR_DIALOG)
    {
        String dir_path;
        makePathFromBreadcrumbs(dir_path);
        dir_path += "/";
        dir_path += _dialog_txt->getText();

        _dialog_success_res = _fs.createDir(dir_path.c_str());
        showResultToast(_dialog_success_res);
    }
    else if (_mode == MODE_RENAME_DIALOG)
    {
        String old_name;
        String new_name;

        makePathFromBreadcrumbs(old_name);
        new_name = old_name;

        old_name += "/";
        old_name += _old_name;
        _old_name = "";

        new_name += "/";
        new_name += _dialog_txt->getText();

        _dialog_success_res = _fs.rename(old_name.c_str(), new_name.c_str());
        showResultToast(_dialog_success_res);
    }

    hideDialog();
}

void FilesContext::keyboardClickHandler()
{
    // uint16_t id = _keyboard->getCurrBtnID();
    // if (id ==)
    // {
    // }

    // Можна оброблювати по ID кнопки, але в даному випадку зручніше оброблювати текст,
    // Тому що ніякі керуючі кнопки не використовуються.

    _dialog_txt->addChars(_keyboard->getCurrBtnTxt().c_str());
}

//-------------------------------------------------------------------------------------------

void FilesContext::prepareFileMoving()
{
    makePathFromBreadcrumbs(_path_from);
    _name_from = _files_list->getCurrItemText();

    _has_moving_file = true;
    _has_copying_file = false;

    hideContextMenu();
}

void FilesContext::prepareFileCopying()
{
    makePathFromBreadcrumbs(_path_from);
    _name_from = _files_list->getCurrItemText();

    _has_moving_file = false;
    _has_copying_file = true;

    hideContextMenu();
}

void FilesContext::pasteFile()
{
    String old_file_path = _path_from;
    old_file_path += "/";
    old_file_path += _name_from;

    String new_file_path;
    makePathFromBreadcrumbs(new_file_path);

    new_file_path += "/";
    new_file_path += _name_from;

    if (_has_moving_file)
    {
        if (_fs.exists(old_file_path.c_str()) && _fs.rename(old_file_path.c_str(), new_file_path.c_str()))
        {
            indexCurDir();
            fillFilesTmpl();
            hideContextMenu();
            showResultToast(true);
        }
        else
            showResultToast(false);
    }
    else if (_has_copying_file)
    {
        if (!_fs.fileExist(old_file_path.c_str()) || !_fs.startCopyFile(old_file_path.c_str(), new_file_path.c_str()))
        {
            showResultToast(false);
        }
        else
        {
            if (old_file_path.equals(new_file_path))
            {
                new_file_path += "_copy";

                while (_fs.fileExist(new_file_path.c_str(), true))
                    new_file_path += "_copy";
            }

            _copy_to_path = new_file_path;

            showCopyingTmpl();
        }
    }

    _has_moving_file = false;
    _has_copying_file = false;

    _path_from = "";
    _name_from = "";
}

void FilesContext::removeFile()
{
    String file_name;
    makePathFromBreadcrumbs(file_name);

    file_name += "/";
    file_name += _files_list->getCurrItemText();

    if (_fs.startRemoving(file_name.c_str()))
        showRemovingTmpl();
}

//-------------------------------------------------------------------------------------------

void FilesContext::update()
{
    if (_mode == MODE_SD_UNCONN)
    {
        if (_input.isReleased(BtnID::BTN_B))
        {
            _input.lock(BtnID::BTN_B, CLICK_LOCK);
            openContextByID(ID_CONTEXT_MENU);
        }

        return;
    }

    if (_input.isReleased(BtnID::BTN_D))
    {
        _input.lock(BtnID::BTN_D, CLICK_LOCK);
        if (_mode == MODE_NAVIGATION)
            showContextMenu();
    }
    else if (_input.isPressed(BtnID::BTN_SAVE))
    {
        _input.lock(BtnID::BTN_SAVE, PRESS_LOCK);

        if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
            saveDialogResult();
    }
    else if (_input.isPressed(BtnID::BTN_EXIT))
    {
        _input.lock(BtnID::BTN_EXIT, PRESS_LOCK);

        if (_mode == MODE_NAVIGATION)
            openContextByID(ID_CONTEXT_MENU);
        else if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
            hideDialog();
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
    else if (_input.isHolded(BtnID::BTN_RIGHT))
    {
        _input.lock(BtnID::BTN_RIGHT, HOLD_LOCK);
        if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
            _keyboard->focusRight();
    }
    else if (_input.isHolded(BtnID::BTN_LEFT))
    {
        _input.lock(BtnID::BTN_LEFT, HOLD_LOCK);
        if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
            _keyboard->focusLeft();
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

    if (_fs.isWorking())
    {
        if ((millis() - _upd_msg_time) > UPD_TRACK_INF_INTERVAL)
        {
            String upd_txt;
            String upd_progress;

            if (_upd_counter > 2)
                _upd_counter = 0;
            else
                ++_upd_counter;

            for (uint8_t i{0}; i < _upd_counter; ++i)
                upd_progress += ".";

            if (_mode == MODE_CANCELING)
            {
                upd_txt = STR_CANCELING;
                upd_txt += upd_progress;
                _msg_lbl->setText(upd_txt);
            }
            else if (_mode == MODE_COPYING)
            {
                _task_progress->setProgress(_fs.getCopyProgress());
                _upd_msg_time = millis();
            }
            else if (_mode == MODE_REMOVING)
            {
                upd_txt = STR_REMOVING;
                upd_txt += upd_progress;
                _msg_lbl->setText(upd_txt);
            }

            _upd_msg_time = millis();
        }
    }
}

void FilesContext::ok()
{
    if (_mode == MODE_NAVIGATION)
        openNextLevel();
    else if (_mode == MODE_CONTEXT_MENU)
    {
        uint16_t id = _context_menu->getCurrItemID();

        if (id == ID_ITEM_UPDATE)
        {
            hideContextMenu();
            indexCurDir();
            fillFilesTmpl();
        }
        else if (id == ID_ITEM_REMOVE)
            removeFile();
        else if (id == ID_ITEM_MOVE)
            prepareFileMoving();
        else if (id == ID_ITEM_COPY)
            prepareFileCopying();
        else if (id == ID_ITEM_PASTE)
            pasteFile();
        else if (id == ID_ITEM_NEW_DIR)
            showDialog(MODE_NEW_DIR_DIALOG);
        else if (id == ID_ITEM_RENAME)
            showDialog(MODE_RENAME_DIALOG);
        else if (id == ID_ITEM_IMPORT)
            startFileServer(FileServer::SERVER_MODE_RECEIVE);
        else if (id == ID_ITEM_EXPORT)
            startFileServer(FileServer::SERVER_MODE_SEND);
        else if (id == ID_ITEM_EXECUTE)
            executeScript();
    }
    else if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
        keyboardClickHandler();
    else if (_mode == MODE_NOTIFICATION)
    {
        hideNotification();
        _mode = MODE_NAVIGATION;
        getLayout()->forcedDraw();
    }
}

void FilesContext::back()
{
    if (_fs.isWorking() && _mode != MODE_CANCELING)
    {
        showCancelingTmpl();
        _fs.cancel();
    }
    else if (_mode == MODE_CONTEXT_MENU)
        hideContextMenu();
    else if (_mode == MODE_NAVIGATION)
        openPrevlevel();
    else if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
        _dialog_txt->removeLastChar();
    else if (_mode == MODE_FILE_SERVER)
        stopFileServer();
}

void FilesContext::up()
{
    if (_mode == MODE_NAVIGATION)
    {
        if (_files_list->focusUp())
        {
            updateFileInfo();
            _scrollbar->scrollUp();
        }
    }
    else if (_mode == MODE_CONTEXT_MENU)
        _context_menu->focusUp();
    else if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
        _keyboard->focusUp();
}

void FilesContext::down()
{
    if (_mode == MODE_NAVIGATION)
    {
        if (_files_list->focusDown())
        {
            updateFileInfo();
            _scrollbar->scrollDown();
        }
    }
    else if (_mode == MODE_CONTEXT_MENU)
        _context_menu->focusDown();
    else if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
        _keyboard->focusDown();
}

void FilesContext::updateFileInfo()
{
    String file_name;
    makePathFromBreadcrumbs(file_name);
    file_name += "/";
    file_name += _files_list->getCurrItemText();

    String str_size;
    double f_size = _fs.getFileSize(file_name.c_str());

    if (f_size < 1024)
    {
        str_size = String(f_size);
        str_size += "b";
    }
    else if (f_size < 1024 * 1024)
    {
        str_size = String(f_size / 1024.0);
        str_size += "kb";
    }
    else
    {
        str_size = String(f_size * 9.53674316e-7);
        str_size += "mb";
    }

    _file_size_lbl->setText(str_size.c_str());
    _file_size_lbl->updateWidthToFit();

    String pos = "[";
    pos += String(_files_list->getCurrItemID());
    pos += "/";
    pos += String(_files.size());
    pos += "]";
    _file_pos_lbl->setText(pos.c_str());
    _file_pos_lbl->updateWidthToFit();
}

void FilesContext::openNextLevel()
{
    String next_dir = "/";
    next_dir += _files_list->getCurrItemText();

    String next_dir_path;
    makePathFromBreadcrumbs(next_dir_path);
    next_dir_path += next_dir;

    if (!_fs.dirExist(next_dir_path.c_str(), true))
        return;

    _breadcrumbs.push_back(next_dir);
    indexCurDir();
    fillFilesTmpl();
}

void FilesContext::openPrevlevel()
{
    if (_breadcrumbs.size() > 0)
    {
        _breadcrumbs.pop_back();
        indexCurDir();
        fillFilesTmpl();
    }
    else
    {
        openContextByID(ID_CONTEXT_MENU);
    }
}

void FilesContext::indexCurDir()
{
    String dir_path;
    makePathFromBreadcrumbs(dir_path);
    _fs.indexAll(_files, dir_path.c_str());
}

void FilesContext::fillFilesTmpl()
{
    _files_list->delWidgets();

    std::vector<MenuItem *> items;

    makeMenuFilesItems(items, 0, _files_list->getItemsNumOnScreen());

    for (size_t i = 0; i < items.size(); ++i)
        _files_list->addItem(items[i]);

    _scrollbar->setValue(0);
    _scrollbar->setMax(_files.size());

    updateFileInfo();
}

void FilesContext::startFileServer(FileServer::ServerMode mode)
{
    SettingsManager sm;
    String ssid = sm.get(STR_PREF_FS_AP_SSID);
    String pwd = sm.get(STR_PREF_FS_AP_PWD);

    if (ssid.isEmpty())
        ssid = STR_DEF_SSID;
    if (pwd.isEmpty())
        pwd = STR_DEF_PWD;

    _server.setSSID(ssid.c_str());
    _server.setPWD(pwd.c_str());
    //
    String cur_path;
    makePathFromBreadcrumbs(cur_path);
    if (_server.begin(cur_path.c_str(), mode))
    {
        QR_Gen gen;
        String addr = _server.getAddress();
        _qr_img_buff = gen.generateQR(addr.c_str(), 3);
        _qr_width = gen.getImageWidth();

        showServerTmpl();
    }
}

void FilesContext::stopFileServer()
{
    if (!_is_back_eabled)
        return;

    _server.stop();

    showFilesTmpl();
    indexCurDir();
    fillFilesTmpl();

    free(_qr_img_buff);
}

void FilesContext::taskDoneHandler(bool result)
{
    showResultToast(result);

    indexCurDir();
    showFilesTmpl();
    fillFilesTmpl();
}

void FilesContext::taskDone(bool result, void *arg)
{
    FilesContext *self = static_cast<FilesContext *>(arg);
    self->taskDoneHandler(result);
}

//-------------------------------------------------------------------------------------------

void FilesContext::makeMenuFilesItems(std::vector<MenuItem *> &items, uint16_t file_pos, uint8_t size)
{
    if (file_pos >= _files.size())
        return;

    uint16_t read_to = file_pos + size;

    if (read_to > _files.size())
        read_to = _files.size();

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

        if (_files[i].isDir())
            item->setImg(_dir_img);
        else if (_files[i].nameEndsWith(STR_LUA_EXT))
            item->setImg(_lua_img);

        lbl->setText(_files[i].getName());
    }
}

//-------------------------------------------------------------------------------------------

void FilesContext::handleNextItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id)
{
    if (!cur_id)
        return;

    makeMenuFilesItems(items, cur_id, size);
}

void FilesContext::onNextItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg)
{
    FilesContext *self = static_cast<FilesContext *>(arg);
    self->handleNextItemsLoad(items, size, cur_id);
}

//

void FilesContext::handlePrevItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id)
{
    // Не отримали ID
    if (!cur_id)
        return;

    uint16_t item_pos = cur_id - 1;

    // Вже перший елемент
    if (!item_pos)
        return;

    // Вирахувати першу позицію звідки потрібно читати список.
    if (cur_id > size)
        item_pos = cur_id - size - 1;
    else
    {
        item_pos = 0;
        // Вирівняти скролбар, якщо меню було завантажене не з першого елемента
        _scrollbar->setValue(cur_id);
    }

    makeMenuFilesItems(items, item_pos, size);
}

void FilesContext::onPrevItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg)
{
    FilesContext *self = static_cast<FilesContext *>(arg);
    self->handlePrevItemsLoad(items, size, cur_id);
}

void FilesContext::showResultToast(bool result)
{
    if (result)
        showToast(STR_SUCCSESS, TOAST_LENGTH_SHORT);
    else
        showToast(STR_FAIL, TOAST_LENGTH_SHORT);
}

void FilesContext::createNotificationObj()
{
    _notification = new Notification(1);
    _notification->setLeftBackColor(TFT_DARKCYAN);
    _notification->setRightBackColor(TFT_DARKCYAN);
    _notification->setTitleText(STR_NOTIFICATION);
    _notification->setRightText(STR_OK);
}

void FilesContext::executeScript()
{
    String file_name;
    makePathFromBreadcrumbs(file_name);
    file_name += "/";
    file_name += _files_list->getCurrItemText();

    size_t f_size = _fs.getFileSize(file_name.c_str());

    if (f_size == 0)
    {
        log_e("Скрипт порожній");
        return;
    }

    char *text_buf;

    if (psramInit())
        text_buf = static_cast<char *>(ps_malloc(f_size + 1));
    else
        text_buf = static_cast<char *>(malloc(f_size + 1));

    if (!text_buf)
    {
        log_e("Помилка виділення пам'яті");
        return;
    }

    hideContextMenu();

    _fs.readFile(file_name.c_str(), text_buf, f_size);
    text_buf[f_size] = '\0';

    _lua_context = new LuaContext();
    if (!_lua_context->execScript(text_buf))
    {
        String msg = _lua_context->getMsg();
        _notification->setMsgText(msg);
        _mode = MODE_NOTIFICATION;
        showNotification(_notification);
        delete _lua_context;
        _lua_context = nullptr;
    }
    else
    {
        _mode = MODE_LUA;
        log_i("%s", STR_LUA_RUNNING);
    }

    free(text_buf);
}
