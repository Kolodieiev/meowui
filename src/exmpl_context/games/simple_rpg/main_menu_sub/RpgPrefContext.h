#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/ui/widget/layout/EmptyLayout.h"
#include "meow/ui/widget/text/TextBox.h"
#include "meow/ui/widget/keyboard/Keyboard.h"

using namespace meow;

namespace simple_rpg
{
    class RpgPrefContext : public IContext
    {
    public:
        RpgPrefContext();
        virtual ~RpgPrefContext() override {}

    protected:
        virtual bool loop() override;
        virtual void update() override;

    private:
        enum WidgetID : uint8_t
        {
            ID_MENU_MAIN = 1,
            ID_ITEM_NICK,
            ID_ITEM_SERV_NAME,
            ID_ITEM_SERV_PWD,
            ID_DIALOG_LBL,
            ID_PWD_TXT,
            ID_KEYBOARD,
        };

        enum StateID : uint8_t // Ідентифікатори стану контексту
        {
            STATE_SD_UNMOUNT = 1,
            STATE_MAIN_MENU,
            STATE_PREF_NICK,
            STATE_PREF_SERV_NAME,
            STATE_PREF_SERV_PWD
        };

        StateID _state_id{STATE_SD_UNMOUNT};

        FixedMenu *_pref_menu; // Вказівник на головне меню налаштувань гри
        //
        TextBox *_dialog_txt; // Поле вводу налаштувань
        Keyboard *_keyboard;  // Клавіатура
        //
        // Шаблон головного меню налаштувань гри
        void showMainMenuTmpl();
        // Обробник кнопок головного меню налаштувань
        void handleMainMenuBtns();
        //
        // Шаблон з повідомленням про помилку монтування SD
        void showSDErrTmpl();
        // Обробник кнопок в стану відсутньої карти пам'яті
        void handleSDUnmountBtns();
        // Формування діалогу вводу нікнейма
        void showPrefNickTmpl();
        // Формування діалогу вводу серверного імені
        void showPrefServNameTmpl();
        // Формування діалогу вводу серверного паролю
        void showPrefServPwdTmpl();

        // Обробка вводу в стані вікна вводу
        void handleDialBtns();
        // Збереження налаштувань
        void saveDialogResult();

        // Додає діалог до UI
        void addDialog(EmptyLayout *layout, const char *title_txt, const char *start_txt);
    };
}