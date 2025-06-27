#pragma once
#include <Arduino.h>
#include "meow/game/DataStream.h"
//
#include "meow/ui/context/IContext.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/ui/widget/scrollbar/ScrollBar.h"
#include "meow/game/IGameScene.h"
#include "meow/game/online/GameServer.h"
#include "meow/game/online/GameClient.h"
#include "meow/manager/wifi/WiFiManager.h"
#include "meow/ui/widget/keyboard/Keyboard.h"
#include "meow/ui/widget/text/TextBox.h"

using namespace meow;

namespace test_server
{
    class TestServerContext : public IContext
    {
    public:
        TestServerContext();
        virtual ~TestServerContext() override;

    protected:
        virtual bool loop() override;
        virtual void update() override;

    private:
        enum WidgetID : uint8_t // Ід віджетів
        {
            ID_MAIN_MENU = 1,
            ID_ROLE_MENU,
            ID_LOBBY_MENU,
            ID_WIFI_LIST_MENU,
            ID_CONTEXT_LBL,
            ID_SSID_TXT,
            ID_PWD_TXT,
            ID_KEYBOARD,
            ID_CLIENT_LIST_MENU,
            ID_LOBBY_CONTEXT_MENU,
            ID_PREF_MAIN_MENU,
        };

        enum ItemID : uint8_t // Ід пунктів меню
        {
            // MAIN
            ID_ITEM_ONLINE = 1,
            ID_ITEM_OFFLINE,
            ID_ITEM_PREF,
            // ROLE
            ID_ITEM_SERVER,
            ID_ITEM_CLIENT,
            // PREF
            ID_ITEM_NICK,
            ID_ITEM_SERV_NAME,
            ID_ITEM_SERV_PWD,
            // CONT_MENU
            ID_ITEM_DISC_CLIENT,
            ID_ITEM_START_GAME,
            ID_ITEM_END_GAME,
            ID_ITEM_CLOSE_LOBBY,
            ID_ITEM_OPEN_LOBBY,
        };

        enum ContextID : uint8_t // Ід контексту
        {
            ID_CONT_MAIN = 1,
            ID_CONT_ROLE,
            ID_CONT_WIFI_LIST,
            ID_CONT_CLIENT_LOBBY,
            ID_CONT_CONN_DIALOG,
            ID_CONT_CONN_TO_AP,
            ID_CONT_GAME,
            //
            ID_CONT_SERVER_LOBBY,
            ID_CONT_LOBBY_CONT_MENU,
            ID_CONT_CLIENT_CONFIRM,
            //
            ID_CONT_PREF_MAIN,
            ID_CONT_PREF_NICK,
            ID_CONT_PREF_SERV_NAME,
            ID_CONT_PREF_SERV_PWD,
        };

        ContextID _context_id{ID_CONT_MAIN};

        // Dialog
        TextBox *_dialog_txt;
        Keyboard *_keyboard;
        void addDialog(EmptyLayout *layout, const char *title_txt, const char *start_txt);
        //
        String _serv_ssid;
        String _serv_pwd;
        // ---------------------------- Main
        const uint16_t IMG_W = 200;
        const uint16_t IMG_H = 40;
        const uint16_t ITEMS_SPASING = 10;
        Image *_img_back_normal;
        Image *_img_back_hover;
        FixedMenu *_main_menu;
        void showMainMenu();
        void handleMainBtns();

        // ---------------------------- Role
        FixedMenu *_role_menu;
        void showSelectRole();
        void handleRoleBtns();

        // ---------------------------- Lobby
        FixedMenu *_lobby_menu;
        const uint8_t SERVER_WIFI_CHANN{6};

        // --------------------------------------------------------------- Client lobby
        String _client_nick;

        GameClient _client;
        //
        FixedMenu *_wifi_list_menu;

        // Список wifi
        void showWifiList();
        void handleWifiListBtns();
        static void wifiScanDoneHandler(void *arg);
        void readWifiScan();
        // Ввід паролю до wifi
        void showConnectDialog();
        // Підключення до wifi
        void showConnToAP();
        void handleConnToAPBtns();
        static void wifiConnDoneHandler(void *arg, wl_status_t conn_status);
        void updateConnectState(wl_status_t conn_status);

        // Підключення до сервера
        void handleServerData(const UdpPacket *packet);
        static void onServerData(const UdpPacket *packet, void *arg);
        void handleServerConn();
        void handleServerDisconn();
        static void serverConnHandler(void *arg);
        static void serverDisconnHandler(void *arg);

        // Вікно лоббі
        void showClientLobby();
        void handleClientLobbyBtns();

        // --------------------------------------------------------------- Server lobby
        GameServer _server;

        // Головне вікно лоббі. Очікування гравців
        FixedMenu *_clients_list_menu;
        void showServerLobby();
        void updateClientsList(const ClientWrapper *disconnected_client = nullptr);
        void handleServerLobbyBtns();
        void openServerLobby();
        // Показати контекстне меню
        FixedMenu *_lobby_context_menu;
        void showServLobbyContMenu();
        void hideServLobbyContMenu();
        void handleLobbyContMenuBtns();

        // Обробка запиту приєднання гравця
        const ClientWrapper *_conn_client_wrap{nullptr};
        ConfirmResultHandler_t _confirm_handler{nullptr};
        //
        // Приєднання клієнта
        static void clientConfirmHandler(const ClientWrapper *cl_wrap, ConfirmResultHandler_t result_handler, void *arg);
        void handleClientConn(const ClientWrapper *cl_wrap, ConfirmResultHandler_t result_handler);
        void showClientConn();
        void handleClientConnBtns();
        // Обробка від'єднання клієнта
        void handleClientDisconn(const ClientWrapper *cl_wrap);
        static void clientDisconnHandler(const ClientWrapper *cl_wrap, void *arg);
        // ---------------------------- Pref
        FixedMenu *_pref_menu;
        void showPrefMain();
        void handlePrefMainBtns();
        //
        void showPrefNick();
        void showPrefServName();
        void showPrefServPwd();
        void handleDialBtns();
        void handlePrefSaveBtns();
        // ---------------------------- Game
        bool _is_server{false};
        bool _is_client{false};

        IGameScene *_scene;
        DataStream _stored_objs{1}; // Перенесення об'єктів між сценами не буде відбуватися. Тому виділяємо тільки 1 байт
        void showMainScene();
        void updateGame();
    };
}