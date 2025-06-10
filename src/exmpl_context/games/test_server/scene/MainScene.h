#pragma once
#include <Arduino.h>
#include "meow/game/IGameScene.h"
#include "../ui/SceneUI.h"

#include "meow/game/online/GameClient.h"
#include "meow/game/online/GameServer.h"
#include "../obj/BallObj.h"
#include "../obj/ClassID.h"

using namespace meow;

namespace test_server
{
    class MainScene : public IGameScene
    {
    public:
        explicit MainScene(DataStream &stored_objs);
        MainScene(DataStream &stored_objs, GameServer &server);
        MainScene(DataStream &stored_objs, GameClient &client);

        virtual ~MainScene();
        virtual void update() override;

    private:
        const size_t SIZE_CMD_MOVE = 1 + 1;   // Команда + напрямок
        const size_t SIZE_CMD_READY = 1;      // Команда
        const size_t SIZE_CMD_SET_ID = 1 + 4; // Команда + id об'єкта

        enum GameCmd : uint8_t
        {
            CMD_READY,   // Готовність обмінюватися даними
            CMD_SET_ID,  // Передача клієнту ідентифікатора його об'єкта
            CMD_INIT,    // Сповіщення клієнтам, що в пакеті знаходяться дані об'єктів. Кожен клієнт повинен прив'язатися до свого об'єкту
            CMD_MOVE,    // Надсилання команди рухати об'єкт
            CMD_UPD_OBJ, // Команда, яка означає, що в даних знаходяться серіалізовані об'єкти
        };

        enum Mode : uint8_t
        {
            MODE_SOLO = 0,
            MODE_SERVER,
            MODE_CLIENT
        };

        Mode _mode;

        // ---------------------------------------------------------------------------------

        GameServer stub_serv; // Об'єкт для заглушки _server.
        GameServer &_server;
        bool _is_clients_connected{false};
        //
        std::unordered_map<uint32_t, IGameObject *> _client_id; // Таблиця в якій зберігається адреса клієнта та вказівник на його об'єкт
                                                                //
        void handleClientData(const ClientWrapper *cl_wrap, const UdpPacket *packet);
        static void onClientData(const ClientWrapper *cl_wrap, const UdpPacket *packet, void *arg);
        //
        void handleClientDisconn(const ClientWrapper *cl_wrap);
        static void onClientDisconn(const ClientWrapper *cl_wrap, void *arg);
        //
        // Виконати дії, повязані зі сервером
        void doServerStuff();

        // -----------------------------------------------------------------------------------------------------------------------------

        GameClient stub_client; // Об'єкт для заглушки _client.
        GameClient &_client;
        uint32_t _client_obj_id{0};

        void handleServerData(UdpPacket *packet);
        static void onServerData(UdpPacket *packet, void *arg);
        //
        void handleServerDisconn();
        static void onServerDisconn(void *arg);
        //
        // Виконати дії, повязані з клієнтом
        void doClientStuff(IGameObject::MovingDirection direction);
        IGameObject *createObjByClass(ClassID id);
        // ---------------------------------------------------------------------------------

        //
        BallObj *_ball; // Ігровий об'єкт серверного гравця

        SceneUI *_ui;

        void init();
        void buildTerrain();
    };
} // namespace test_server
