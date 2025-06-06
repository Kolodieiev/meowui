#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"      //
#include "meow/game/IGameScene.h"          //
#include "meow/game/DataStream.h"          // Необхідний для передавання серіалізованих об'єктів між ігровими рівнями
#include "meow/ui/widget/menu/FixedMenu.h" //
//

// Підключення субмодулів
#include "./main_menu_sub/RpgPrefContext.h"

using namespace meow;

namespace simple_rpg
{
    class SimpleRpgContext : public IContext
    {
    public:
        SimpleRpgContext();
        virtual ~SimpleRpgContext() override;

    protected:
        virtual bool loop() override;
        virtual void update() override;

    private:
        enum WidgetID : uint8_t // Ідентифікатори віджетів
        {
            ID_MENU_MAIN = 1, // Ідентифікатор головного меню
            ID_ITEM_ONLINE,   // Ідентифікатори віджетів елементів списку головного меню
            ID_ITEM_OFFLINE,
            ID_ITEM_PREF,
            //
        };

        enum StateID : uint8_t // Ідентифікатори стану контексту
        {
            STATE_MAIN_MENU = 1,
            STATE_GAME,
            STATE_PREF_SUB_CXT,
        };

        StateID _state_id{STATE_MAIN_MENU}; // Змінна, що зберігає ідентифікатор поточного стану контексту

        // ---------------------------------------------------------------------------------------- Головне меню
        const uint16_t IMG_W = 200;        // Ширина фонового зображення елементів списку
        const uint16_t IMG_H = 40;         // Висота фонового зображення елементів списку
        const uint16_t ITEMS_SPASING = 10; // Розмір відступу між елементами списку меню
        Image *_img_back_normal;           // Закешований вказівник на фонове зображення елементів списку, коли елемент не у фокусі
        Image *_img_back_hover;            // Закешований вказівник на фонове зображення елементів списку, коли елемент у фокусі
        FixedMenu *_main_menu;             // Закешований вказівник на віджет головного меню

        RpgPrefContext *_pref_context;

        // Метод генерації шаблону головного меню
        void showMainMenuTmpl();
        // Змінює фонове зображення поточного пункту головного меню на "звичайне"
        void setItemBackNormal();
        // Змінює фонове зображення поточного пункту головного меню на "у фокусі"
        void setItemBackHover();
        // Обробник вводу головного меню гри
        void handleMainMenuBtns();
        // Метод, що відкриває субконтекст з налаштуваннями гри.
        void openPrefSubContext();
        // ---------------------------------------------------------------------------------------- Вибір ролі
        String _client_nick;
        String _serv_ssid;
        String _serv_pwd;
        // ----------------------------------------------------------------------------------------
        IGameScene *_scene;    // Вказівник на поточну ігрову сцену
        DataStream _stored_objs{1}; // Об'єкт для перенесення об'єктів між сценами
        void updateGame();
        void showMainScene();
        // ----------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------
    };
} // namespace simple_rpg
