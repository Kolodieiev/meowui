#pragma GCC optimize("O3")

#include "Mp3Context.h"



#include "./res/play.h"
#include "./res/pause.h"
#include "./res/speaker.h"
#include "./res/forward.h"
#include "./res/rewind.h"
#include "./res/clock.h"

#include "../WidgetCreator.h"
#include "meow/ui/widget/layout/EmptyLayout.h"

#include "meow/manager/sd/SD_Manager.h"

const char STR_PLAYLIST_PREF[] = "Playlist";
const char STR_TRACK_NAME_PREF[] = "TrackName";
const char STR_VOLUME_PREF[] = "Volume";
const char STR_TRACK_POS_PREF[] = "TrackPos";
const char STR_TRACK_TIME_PREF[] = "TrackTime";

const char ROOT_PATH[] = "/music";
const char AUDIO_EXT[] = ".mp3";

bool Mp3Context::loop()
{
    _audio.loop();
    return true;
}

void Mp3Context::savePref()
{
    _settings.set(STR_VOLUME_PREF, String(_volume).c_str());
    _settings.set(STR_TRACK_POS_PREF, String(_track_pos).c_str());
    _settings.set(STR_TRACK_TIME_PREF, String(_track_time).c_str());
    _settings.set(STR_PLAYLIST_PREF, _playlist_name.c_str());
    _settings.set(STR_TRACK_NAME_PREF, _track_name.c_str());
}

//-------------------------------------------------------------------------------------------

Mp3Context::Mp3Context()
{
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    if (!_sd.isMounted())
    {
        showSDErrTmpl();
        return;
    }

    _volume = atoi(_settings.get(STR_VOLUME_PREF).c_str());
    _track_pos = atoi(_settings.get(STR_TRACK_POS_PREF).c_str());
    _track_time = atoi(_settings.get(STR_TRACK_TIME_PREF).c_str());
    _playlist_name = _settings.get(STR_PLAYLIST_PREF);
    _track_name = _settings.get(STR_TRACK_NAME_PREF);

    if (_volume == 0)
        _volume = 5;

    _audio.setTone(3, -1, -3);
    _audio.setVolumeSteps(31);
    _audio.setVolume(_volume);

    String mono_mode = _settings.get(STR_PREF_MONO_AUDIO);
    if (mono_mode.equals("1"))
        _audio.forceMono(true);

    indexPlaylists();
    showPlaylistsTmpl();
    fillPlaylists();
}

//-------------------------------------------------------------------------------------------

void Mp3Context::showPlaying()
{
    uint8_t paddings{10};

    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    _track_name_lbl = new Label(ID_TRACK_NAME);
    layout->addWidget(_track_name_lbl);
    _track_name_lbl->setText(_track_name);
    _track_name_lbl->setFontID(4);
    _track_name_lbl->setAlign(IWidget::ALIGN_CENTER);
    _track_name_lbl->setGravity(IWidget::GRAVITY_CENTER);
    _track_name_lbl->setBackColor(TFT_BLACK);
    _track_name_lbl->setTextColor(TFT_ORANGE);
    _track_name_lbl->setWidth(D_WIDTH - TFT_CUTOUT * 2);
    _track_name_lbl->setHeight(20);
    _track_name_lbl->setTicker(true);
    _track_name_lbl->setPos(getCenterX(_track_name_lbl), 0);

    _progress = new ProgressBar(ID_PROGRESS);
    layout->addWidget(_progress);
    _progress->setBackColor(TFT_BLACK);
    _progress->setProgressColor(TFT_ORANGE);
    _progress->setBorderColor(TFT_WHITE);
    _progress->setMax(9999);
    _progress->setWidth(D_WIDTH - 20);
    _progress->setHeight(10);
    _progress->setProgress(1);
    _progress->setPos(paddings, D_HEIGHT - TFT_CUTOUT - _progress->getHeight());

    _cur_track_time_lbl = _track_name_lbl->clone(ID_CUR_TRACK_TIME);
    layout->addWidget(_cur_track_time_lbl);
    _cur_track_time_lbl->setText(STR_ZERO_TRACK_TIME);
    _cur_track_time_lbl->setAlign(IWidget::ALIGN_END);
    _cur_track_time_lbl->initWidthToFit();
    _cur_track_time_lbl->setBackColor(COLOR_MAIN_BACK);
    _cur_track_time_lbl->setPos(paddings, _progress->getYPos() - 25);
    _cur_track_time_lbl->setTicker(false);

    _gen_track_time_lbl = _cur_track_time_lbl->clone(ID_GEN_TRACK_TIME);
    layout->addWidget(_gen_track_time_lbl);
    _gen_track_time_lbl->setText(STR_ZERO_TRACK_TIME);
    _gen_track_time_lbl->initWidthToFit();
    _gen_track_time_lbl->setPos(D_WIDTH - _cur_track_time_lbl->getWidth() - paddings, _cur_track_time_lbl->getYPos());

    //

    _play_btn = new Image(ID_PLAY_BTN);
    layout->addWidget(_play_btn);
    _play_btn->init(32, 32);
    _play_btn->setSrc(PAUSE_IMG);
    _play_btn->setBackColor(COLOR_MAIN_BACK);
    _play_btn->setTranspColor(TFT_TRANSPARENT);
    _play_btn->setPos(getCenterX(_play_btn), getCenterY(_play_btn) - TFT_CUTOUT / 2);

    Image *forward_img = new Image(ID_FORWARD_IMG);
    layout->addWidget(forward_img);
    forward_img->init(24, 24);
    forward_img->setSrc(FORWARD_IMG);
    forward_img->setBackColor(COLOR_MAIN_BACK);
    forward_img->setTranspColor(TFT_TRANSPARENT);
    forward_img->setPos(_play_btn->getXPos() + 32 * 2, _play_btn->getYPos() + 4);

    Image *rewind_img = new Image(ID_REWIND_IMG);
    layout->addWidget(rewind_img);
    rewind_img->init(24, 24);
    rewind_img->setSrc(REWIND_IMG);
    rewind_img->setBackColor(COLOR_MAIN_BACK);
    rewind_img->setTranspColor(TFT_TRANSPARENT);
    rewind_img->setPos(_play_btn->getXPos() - 32 - 24, _play_btn->getYPos() + 4);

    //

    _volume_lbl = new Label(ID_VOLUME_LBL);
    layout->addWidget(_volume_lbl);
    _volume_lbl->setText(String(_volume));
    _volume_lbl->setAlign(IWidget::ALIGN_END);
    _volume_lbl->setGravity(IWidget::GRAVITY_CENTER);
    _volume_lbl->setHeight(15);
    _volume_lbl->setWidth(20);
    _volume_lbl->setBackColor(COLOR_MAIN_BACK);
    _volume_lbl->setTextColor(TFT_ORANGE);
    _volume_lbl->setPos(getCenterX(_volume_lbl) + _volume_lbl->getWidth() / 2, _progress->getYPos() - _volume_lbl->getHeight() - 5);

    Image *volume_img = new Image(ID_VOLUME_IMG);
    layout->addWidget(volume_img);
    volume_img->init(16, 16);
    volume_img->setSrc(SPEAKER_IMG);
    volume_img->setBackColor(COLOR_MAIN_BACK);
    volume_img->setTranspColor(TFT_TRANSPARENT);
    volume_img->setPos(getCenterX(volume_img) - volume_img->getWidth() / 2, _volume_lbl->getYPos());

    _mode = MODE_AUDIO_PLAY;

    setLayout(layout);
}

void Mp3Context::showTracksTmpl()
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    _tracks_list = creator.getDynamicMenu(ID_D_MENU);
    layout->addWidget(_tracks_list);
    _tracks_list->setWidth(D_WIDTH - SCROLLBAR_WIDTH);
    _tracks_list->setHeight(D_HEIGHT - TFT_CUTOUT * 2);
    _tracks_list->setItemHeight((_tracks_list->getHeight() - 2) / TRACKS_ITEMS_NUM);
    _tracks_list->setPos(0, TFT_CUTOUT);

    _tracks_list->setOnNextItemsLoadHandler(onNextItemsLoad, this);
    _tracks_list->setOnPrevItemsLoadHandler(onPrevItemsLoad, this);

    _scrollbar = new ScrollBar(ID_SCROLL);
    layout->addWidget(_scrollbar);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(D_HEIGHT - TFT_CUTOUT * 2);
    _scrollbar->setPos(D_WIDTH - SCROLLBAR_WIDTH, TFT_CUTOUT);

    _mode = MODE_TRACK_SEL;

    setLayout(layout);
}

void Mp3Context::showPlaylistsTmpl()
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    _playlists_list = new FixedMenu(ID_F_MENU);
    layout->addWidget(_playlists_list);
    _playlists_list->setBackColor(COLOR_MENU_ITEM);
    _playlists_list->setWidth(D_WIDTH - SCROLLBAR_WIDTH);
    _playlists_list->setHeight(D_HEIGHT - TFT_CUTOUT * 2);
    _playlists_list->setItemHeight((D_HEIGHT - TFT_CUTOUT - 2) / PLAYLIST_ITEMS_NUM);
    _playlists_list->setPos(0, TFT_CUTOUT);

    _scrollbar = new ScrollBar(ID_SCROLL);
    layout->addWidget(_scrollbar);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(D_HEIGHT - TFT_CUTOUT * 2);
    _scrollbar->setPos(D_WIDTH - SCROLLBAR_WIDTH, TFT_CUTOUT);

    if (!_track_name.isEmpty())
    {
        MenuItem *cont_item = creator.getMenuItem(ID_CONT_ITEM);
        _playlists_list->addItem(cont_item);

        Label *cont_lbl = creator.getItemLabel(STR_CONTINUE, 4, 2);
        cont_item->setLbl(cont_lbl);
    }

    _mode = MODE_PLST_SEL;

    setLayout(layout);
}

void Mp3Context::fillPlaylists()
{
    std::vector<MenuItem *> items;
    makeMenuPlaylistsItems(items);

    uint16_t size = items.size();

    for (size_t i = 0; i < size; ++i)
        _playlists_list->addItem(items[i]);

    _scrollbar->setValue(0);
    _scrollbar->setMax(_playlists_list->getSize());
}

void Mp3Context::makeMenuPlaylistsItems(std::vector<MenuItem *> &items)
{
    WidgetCreator creator;
    items.clear();

    uint16_t playlist_num = _playlists.size();
    items.reserve(playlist_num);

    for (uint16_t i = 0, counter = ID_CONT_ITEM; i < playlist_num; ++i)
    {
        ++counter;
        MenuItem *item = creator.getMenuItem(counter);
        items.push_back(item);

        Label *lbl = new Label(1);
        item->setLbl(lbl);
        lbl->setTickerInFocus(true);

        lbl->setText(_playlists[i].getName());
    }
}

void Mp3Context::fillTracks(uint16_t track_pos)
{
    _tracks_list->delWidgets();
    size_t pl_sz = _tracks.size();

    if (pl_sz > 0 && track_pos >= pl_sz)
        --track_pos;

    std::vector<MenuItem *> items;
    makeMenuTracksItems(items, track_pos, _tracks_list->getItemsNumOnScreen());

    size_t items_size = items.size();

    for (size_t i = 0; i < items_size; ++i)
        _tracks_list->addItem(items[i]);

    _scrollbar->setMax(pl_sz);
    _scrollbar->setValue(track_pos);
}

void Mp3Context::makeMenuTracksItems(std::vector<MenuItem *> &items, uint16_t file_pos, uint8_t size)
{
    if (file_pos >= _tracks.size())
        return;

    uint16_t read_to = file_pos + size;

    if (read_to > _tracks.size())
        read_to = _tracks.size();

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

        lbl->setText(_tracks[i].getName());
    }
}

void Mp3Context::showPlMenu()
{
    _tracks_list->disable();

    WidgetCreator creator;
    _context_menu = new FixedMenu(ID_PL_MENU);
    getLayout()->addWidget(_context_menu);
    _context_menu->setBackColor(COLOR_MENU_ITEM);
    _context_menu->setBorderColor(TFT_ORANGE);
    _context_menu->setBorder(true);
    _context_menu->setItemHeight(20);
    _context_menu->setWidth(120);
    _context_menu->setHeight(44);
    _context_menu->setPos(D_WIDTH - _context_menu->getWidth(), D_HEIGHT - _context_menu->getHeight() - TFT_CUTOUT);

    if (_tracks_list->getCurrItemID() != 0)
    {
        MenuItem *del_item = creator.getMenuItem(ID_ITEM_DEL);
        _context_menu->addItem(del_item);

        Label *upd_lbl = creator.getItemLabel(STR_DELETE, 4, 2);
        del_item->setLbl(upd_lbl);
    }

    _mode = MODE_PLST_MENU;
}

void Mp3Context::hidePlMenu()
{
    getLayout()->delWidgetByID(ID_PL_MENU);
    _mode = MODE_TRACK_SEL;
    _tracks_list->enable();
}

void Mp3Context::showSDErrTmpl()
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    _msg_lbl = creator.getStatusMsgLable(ID_MSG_LBL, STR_SD_ERR);
    layout->addWidget(_msg_lbl);

    _mode = MODE_SD_UNCONN;
    setLayout(layout);
}

void Mp3Context::updateTrackPos()
{
    if (_track_pos > 0)
    {
        if (_track_pos >= _tracks.size())
            _track_pos = _tracks.size() - 1;
    }
}

//-------------------------------------------------------------------------------------------

void Mp3Context::update()
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

    if (_input.isPressed(BtnID::BTN_RIGHT))
    {
        _input.lock(BtnID::BTN_RIGHT, PRESS_LOCK);
        rightPressed();
    }
    else if (_input.isPressed(BtnID::BTN_LEFT))
    {
        _input.lock(BtnID::BTN_LEFT, PRESS_LOCK);
        leftPressed();
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
    else if (_input.isReleased(BtnID::BTN_D))
    {
        _input.lock(BtnID::BTN_D, PRESS_LOCK);
        openMenu();
    }
    else if (_input.isPressed(BtnID::BTN_EXIT))
    {
        _input.lock(BtnID::BTN_EXIT, PRESS_LOCK);
        backPressed();
    }

    if (_mode == MODE_AUDIO_PLAY)
    {
        if (_audio.isRunning())
        {
            if (_is_new_track && updateTrackDuration())
            {
                _track_name_lbl->setText(_track_name);
                _is_new_track = false;
            }

            if (millis() - _upd_msg_time > UPD_TRACK_INF_INTERVAL)
            {
                updateTrackTime();
                _upd_msg_time = millis();
            }
        }
        // Якщо трек скінчився самостійно
        else if (_is_playing)
        {
            // Намагаємося перемкнути
            if (playNext())
            {
                _is_new_track = true;
            }
            // Якщо не вдалося змінити трек з 3х спроб, зупинити плеєр
            else if (_attempt_to_play_next_counter == 3)
            {
                setStopState();
            }
            else
                ++_attempt_to_play_next_counter;
        }
    }
}

//-------------------------------------------------------------------------------------------

void Mp3Context::leftPressed()
{
    _audio.setTimeOffset(-20);
}

void Mp3Context::rightPressed()
{
    _audio.setTimeOffset(20);
}

void Mp3Context::left()
{
    if (_mode == MODE_AUDIO_PLAY)
        playPrev();
}

void Mp3Context::right()
{
    if (_mode == MODE_AUDIO_PLAY)
        playNext();
}

bool Mp3Context::playNext()
{
    if (_track_pos >= _tracks.size())
        return false;

    _track_name = _tracks[_track_pos + 1].getName();

    if (!playTrack(false))
        return false;

    ++_track_pos;
    return true;
}

bool Mp3Context::playPrev()
{
    if (_tracks.empty() || _track_pos == 0)
        return false;

    _track_name = _tracks[_track_pos - 1].getName();

    if (!playTrack(false))
        return false;

    --_track_pos;
    return true;
}

//-------------------------------------------------------------------------------------------

bool Mp3Context::playTrack(bool contn)
{
    if (contn)
    {
        if ((_playlist_name.isEmpty() || _track_name.isEmpty()))
            return false;
    }
    else
    {
        _track_time = 0;
    }

    String track_path = getTrackPath(_playlist_name.c_str(), _track_name.c_str());

    if (!_audio.connecttoFS(track_path.c_str(), _track_time))
    {
        if (_mode == MODE_AUDIO_PLAY)
            setStopState();
        return false;
    }

    _is_new_track = true;
    _is_playing = true;
    showPlaying();
    return true;
}

void Mp3Context::volumeUp()
{
    if (_volume < _audio.maxVolume() - 2)
    {
        if (_volume < 10)
            _volume += 1;
        else
            _volume += 2;

        _audio.setVolume(_volume);
    }

    _volume_lbl->setText(String(_volume));
}

void Mp3Context::volumeDown()
{
    if (_volume > 1)
    {
        if (_volume > 10)
            _volume -= 2;
        else
            _volume -= 1;

        _audio.setVolume(_volume);
    }

    _volume_lbl->setText(String(_volume));
}

void Mp3Context::setStopState()
{
    _audio.stopSong();

    _is_playing = false;
    _track_pos = 0;
    _track_name = "";

    _play_btn->setSrc(PLAY_IMG);
    _track_name_lbl->setText(STR_STOPPED);
    _cur_track_time_lbl->setText(STR_ZERO_TRACK_TIME);
    _gen_track_time_lbl->setText(STR_ZERO_TRACK_TIME);
    _progress->reset();
}

bool Mp3Context::updateTrackDuration()
{
    uint32_t duration = _audio.getAudioFileDuration();

    if (duration == 0)
        return false;

    uint32_t minutes = floor((float)duration / 60);
    String track_time{""};

    if (minutes < 100)
        track_time += "0";

    if (minutes < 10)
        track_time += "0";

    track_time += minutes;

    track_time += ":";

    uint32_t sec = duration - minutes * 60;
    if (sec < 10)
        track_time += "0";

    track_time += sec;

    _gen_track_time_lbl->setText(track_time);

    _progress->setProgress(0);
    _progress->setMax(duration);

    return true;
}

void Mp3Context::updateTrackTime()
{
    _track_time = _audio.getAudioCurrentTime();

    uint32_t minutes = floor((float)_track_time / 60);
    String track_time_str;

    if (minutes < 100)
        track_time_str += "0";

    if (minutes < 10)
        track_time_str += "0";

    track_time_str += String(minutes);

    track_time_str += ":";

    uint32_t sec = _track_time - minutes * 60;

    if (sec < 10)
        track_time_str += "0";

    track_time_str += String(sec);

    _cur_track_time_lbl->setText(track_time_str);

    _progress->setProgress(_track_time);
}

void Mp3Context::up()
{
    if (_mode == MODE_PLST_SEL)
    {
        _playlists_list->focusUp();
        _scrollbar->scrollUp();
    }
    else if (_mode == MODE_TRACK_SEL)
    {
        _tracks_list->focusUp();
        _scrollbar->scrollUp();
    }
    else if (_mode == MODE_AUDIO_PLAY)
    {
        volumeUp();
    }
    else if (_mode == MODE_PLST_MENU)
    {
        _context_menu->focusUp();
    }
}

void Mp3Context::down()
{
    if (_mode == MODE_PLST_SEL)
    {
        _playlists_list->focusDown();
        _scrollbar->scrollDown();
    }
    else if (_mode == MODE_TRACK_SEL)
    {
        _tracks_list->focusDown();
        _scrollbar->scrollDown();
    }
    else if (_mode == MODE_AUDIO_PLAY)
    {
        volumeDown();
    }
    else if (_mode == MODE_PLST_MENU)
    {
        _context_menu->focusDown();
    }
}

void Mp3Context::ok()
{
    if (_mode == MODE_PLST_SEL)
    {
        uint16_t item_ID = _playlists_list->getCurrItemID();

        if (item_ID == ID_CONT_ITEM)
        {
            indexTracks();
            playTrack(true);
        }
        else
        {
            _playlist_name = _playlists_list->getCurrItemText();
            _track_pos = 0;
            indexTracks();
            showTracksTmpl();
            fillTracks(_track_pos);
        }
    }
    else if (_mode == MODE_TRACK_SEL)
    {
        if (_tracks_list->getSize() > 0)
        {
            _track_name = _tracks_list->getCurrItemText();
            _track_pos = _tracks_list->getCurrItemID() - 1;
            playTrack(false);
        }
    }
    else if (_mode == MODE_AUDIO_PLAY)
    {
        if (_track_name != "")
        {
            _audio.pauseResume();

            if (_audio.isRunning())
            {
                _play_btn->setSrc(PAUSE_IMG);
                _is_playing = true;
            }
            else
            {
                _play_btn->setSrc(PLAY_IMG);
                _is_playing = false;
            }
        }
    }
    else if (_mode == MODE_PLST_MENU)
    {
        uint16_t id = _context_menu->getCurrItemID();

        if (id == ID_ITEM_DEL)
        {
            _track_name = _tracks_list->getCurrItemText();
            if (_track_name.isEmpty())
                return;

            String path_to_rem = getTrackPath(_playlist_name.c_str(), _track_name.c_str());

            if (_fs.rmFile(path_to_rem.c_str(), true))
            {
                if (_tracks_list->getCurrItemID() - 2 > -1)
                    _track_pos = _tracks_list->getCurrItemID() - 2;
                else
                    _track_pos = 0;

                hidePlMenu();
                indexTracks();
                updateTrackPos();
                fillTracks(_track_pos);
            }
        }
    }
}

void Mp3Context::indexPlaylists()
{
    String playlists_path = ROOT_PATH;
    _fs.indexDirs(_playlists, playlists_path.c_str());
}

void Mp3Context::indexTracks()
{
    if (_playlist_name.isEmpty())
        return;

    String playlist_path = ROOT_PATH;
    playlist_path += "/";
    playlist_path += _playlist_name;
    _fs.indexFilesExt(_tracks, playlist_path.c_str(), AUDIO_EXT);
}

String Mp3Context::getTrackPath(const char *dir_name, const char *track_name)
{
    String track_path = ROOT_PATH;
    track_path += "/";
    track_path += _playlist_name;
    track_path += "/";
    track_path += _track_name;
    return track_path;
}

void Mp3Context::handleNextItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id)
{
    if (!cur_id)
        return;

    makeMenuTracksItems(items, cur_id, size);
}

void Mp3Context::onNextItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg)
{
    Mp3Context *self = static_cast<Mp3Context *>(arg);
    self->handleNextItemsLoad(items, size, cur_id);
}

void Mp3Context::handlePrevItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id)
{
    if (!cur_id)
        return;

    uint16_t item_pos = cur_id - 1;

    if (!item_pos)
        return;

    if (cur_id > size)
    {
        item_pos = cur_id - size - 1;
    }
    else
    {
        item_pos = 0;
        _scrollbar->setValue(cur_id);
    }

    makeMenuTracksItems(items, item_pos, size);
}

void Mp3Context::onPrevItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg)
{
    Mp3Context *self = static_cast<Mp3Context *>(arg);
    self->handlePrevItemsLoad(items, size, cur_id);
}

void Mp3Context::openMenu()
{
    if (_mode == MODE_TRACK_SEL)
        showPlMenu();
}

void Mp3Context::back()
{
    if (_mode == MODE_PLST_SEL)
    {
        openContextByID(ID_CONTEXT_MENU);
    }
    else if (_mode == MODE_TRACK_SEL)
    {
        indexPlaylists();
        showPlaylistsTmpl();
        fillPlaylists();
    }
    else if (_mode == MODE_PLST_MENU)
    {
        hidePlMenu();
    }
}

void Mp3Context::backPressed()
{
    if (_mode == MODE_AUDIO_PLAY)
    {
        _audio.stopSong();
        savePref();
        showTracksTmpl();
        fillTracks(_track_pos);
    }
}
