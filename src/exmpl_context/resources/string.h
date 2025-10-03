#pragma once
#include <Arduino.h>

const char STR_AUTHOR[] = "Автор: @Kolodieiev";
const char STR_FIRM_VERSION[] = "Версія прошивки: 1.0.0";

const char STR_MENU[] = "Меню";
const char STR_SELECT[] = "Вибір";
const char STR_SAVE[] = "Зберегти";
const char STR_BACK[] = "Назад";
const char STR_START[] = "Розпочати";
const char STR_STOP[] = "Зупинити";
const char STR_LIGHT[] = "Підсвітка";
const char STR_CANCEL[] = "Відмінити";
const char STR_ERASE[] = "Стерти";
const char STR_UPDATING[] = "Оновлення";
const char STR_COPYING[] = "Копіювання";
const char STR_REMOVING[] = "Видалення";
const char STR_CANCELING[] = "Відміна";
const char STR_PAUSE[] = "Пауза";
const char STR_EXIT[] = "Вихід";
const char STR_PASTE[] = "Вставити";
const char STR_RENAME[] = "Перейменувати";
const char STR_EXECUTE[] = "Виконати";
const char STR_COPY[] = "Копіювати";
const char STR_UPDATE[] = "Оновити";
const char STR_DELETE[] = "Видалити";
const char STR_MOVE[] = "Перемістити";
const char STR_SUCCESS[] = "Успішно";
const char STR_FAIL[] = "Невдача";
const char STR_CONTINUE[] = "Продовжити";
const char STR_IMPORT[] = "Отримати";
const char STR_EXPORT[] = "Поділитися";
const char STR_CONNECT_TO[] = "Приєднатися";
const char STR_CONFIRM[] = "Прийняти";
const char STR_REJECT[] = "Відхилити";
const char STR_OK[] = "OK";
const char STR_NOTIFICATION[] = "Повідомлення";
const char STR_CHANGE[] = "Змінити";
const char STR_NONE[] = "NONE";
//
const char STR_NEW_DIR[] = "Новий кат.";

// splash context
const char STR_SD_LBL[] = "Ініціалізація SD";
const char STR_PSRAM_LBL[] = "Ініціалізація PSRAM";

// home
const char STR_TORCH[] = "Ліхтарик";

// menu
const char STR_LUA_ITEM[] = "Lua";
const char STR_MUSIC_ITEM[] = "Музика";
const char STR_CALC_ITEM[] = "Калькулятор";
const char STR_READER_ITEM[] = "Читалка";
const char STR_FILES_ITEM[] = "Файли";
const char STR_GAME_ITEM[] = "Ігри";
const char STR_WIFI_ITEM[] = "Підключення";
const char STR_PREFERENCES_ITEM[] = "Налаштування";
const char STR_FIRMWARE_ITEM[] = "Прошивка";
// games
const char STR_SOKOBAN_ITEM[] = "Комірник";
const char STR_TEST_SERV_ITEM[] = "Тестовий сервер";
const char STR_SIMPLE_RPG_ITEM[] = "Проста RPG";
const char STR_MULTI_GAME[] = "Мультиплеєр";
const char STR_SOLO_GAME[] = "Один гравець";
const char STR_PREF[] = "Налаштування";
//
const char MON_STR[] = "Понеділок";
const char TUE_STR[] = "Вівторок";
const char WED_STR[] = "Середа";
const char THU_STR[] = "Четвер";
const char FRI_STR[] = "П'ятниця";
const char SAT_STR[] = "Субота";
const char SUN_STR[] = "Неділя";

const char *const DAY_OF_WEEK[] = {
    MON_STR,
    TUE_STR,
    WED_STR,
    THU_STR,
    FRI_STR,
    SAT_STR,
    SUN_STR};

const char EMPTY_TIME[] = "00:00";
const char EMPTY_DATE[] = "00.00.0000";
const char EMPTY_DAY[] = "------";
const char EMPTY_BAT[] = "0.00";

// PREF
const char STR_PREF_BRIGHT[] = "brightness";
const char STR_PREF_MONO_AUDIO[] = "forcemono";
const char STR_PREF_FS_AP_SSID[] = "fs_ap_ssid";
const char STR_PREF_FS_AP_PWD[] = "fs_ap_pwd";
//
const char STR_BRIGHT[] = "Яскравість";
const char STR_AUDIO_MONO[] = "Монозвук";
const char STR_WATCH[] = "Годинник";
const char STR_FILE_SERVER[] = "Файловий сервер";
//
const char STR_AP_NAME[] = "SSID:";
const char STR_AP_PWD[] = "PWD:";
const char STR_DEF_SSID[] = "lilka";
const char STR_DEF_PWD[] = "12345678";

//
const char STR_STOPPED[] = "Зупинено";

const char STR_ZERO_TRACK_TIME[] = "000:00";

//
const char STR_SD_ERR[] = "SD не примонтовано";
//
const char STR_UPDATE_FIRMWARE[] = "Оновити прошивку";
const char STR_FIRMWARE_UPDATING[] = "Очікуйте. Відбувається оновлення.";

//
const char STR_LVL[] = "Рівень ";
//
const char EMPTY_READ_PROGRESS[] = "0000/0000";
//
const char STR_LUA_RUNNING[] = "LuaVM працює";

//

const char STR_TEST_GAME_NICK[] = "test_game_nick";
const char STR_TEST_GAME_C_PWD[] = "test_game_c_pwd";
const char STR_TEST_GAME_S_PWD[] = "test_game_s_pwd";
const char STR_TEST_GAME_S_NAME[] = "test_game_s_name";
const char STR_NICKNAME[] = "Нікнейм";
const char STR_ENTER_NICK[] = "Введіть нікнейм";
const char STR_SERV_NAME[] = "Назва сервера";
const char STR_ENTER_SERV_NAME[] = "Введіть назву сервера";
const char STR_SERV_PWD[] = "Пароль від сервера";
const char STR_ENTER_SERV_PWD[] = "Введіть пароль від сервера";
