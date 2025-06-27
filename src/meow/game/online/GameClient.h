#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <AsyncUDP.h>
#include "UdpPacket.h"

namespace meow
{
    /**
     * @brief Тип обробника, який може бути викликано клієнтом у разі втрати зв'язку з сервером.
     *
     */
    typedef std::function<void(void *arg)> ServerDisconnHandler_t;

    /**
     * @brief Тип обробника, який може бути викликано клієнтом після встановлення зв'язку з сервером.
     *
     */
    typedef std::function<void(void *arg)> ServerConnectedHandler_t;

    /**
     * @brief Тип обробника, який може бути викликано клієнтом після отримання пакета даних від сервера.
     * Об'єкт UdpPacket не потрібно видаляти самостійно.
     *
     */
    typedef std::function<void(UdpPacket *packet, void *arg)> ServerDataHandler_t;

    class GameClient
    {
    public:
        GameClient();
        ~GameClient();

        /**
         * @brief Перечислення, що містить значення станів клієнта.
         *
         */
        enum ClientStatus : uint8_t
        {
            STATUS_IDLE = 0,     // В очікуванні.
            STATUS_CONNECTED,    // Приєднано до сервера.
            STATUS_DISCONNECTED, // З'єднання з сервером втрачено.
            STATUS_WRONG_SERVER, // Некоректний сервер.
            STATUS_WRONG_NAME,   // Відмовлено в авторизації.
            STATUS_SERVER_BUSY,  // Сервер зайнятий обробкою інших запитів.
        };

        /**
         * @brief Встановлює ім'я клієнта.
         *
         * @param name Ім'я клієнта.
         */
        void setName(const char *name) { _name = name; }

        /**
         * @brief Повертає вказівник на поточне ім'я клієнта.
         *
         * @return const char*
         */
        const char *getName() const { return _name.c_str(); }

        /**
         * @brief Встановлює ідентифікатор сервера, до якого очікується підключення.
         *
         * @param id Ідентифікатор сервера.
         */
        void setServerID(const char *id) { _server_id = id; }

        /**
         * @brief Запускає процедуру підключення до сервера.
         *
         * @param host_ip IP-адреса сервера.
         * @return true - Якщо процедуру підключення запущено успішно.
         * @return false - Інакше.
         */
        bool connect(const char *host_ip = "192.168.4.1");

        /**
         * @brief Скидає всі обробники, від'єднує від сервера та звільняє зайняті ресурси.
         * Модуль WiFi не вимикається.
         *
         */
        void disconnect();

        /**
         * @brief Надсилає пакет на сервер, з яким встановлено з'єднання.
         *
         * @param packet Пакет, що буде надіслано на сервер.
         */
        void sendPacket(const UdpPacket &packet);

        /**
         * @brief Формує та надсилає пакет на сервер, з яким встановлено з'єднання.
         *
         * @param type Тип пакету.
         * @param data Дані пакету.
         * @param data_size Розмір даних.
         */
        void send(UdpPacket::PacketType type, const void *data, size_t data_size);

        /**
         * @brief Повертає поточний статус клієнта.
         *
         * @return ClientStatus
         */
        ClientStatus getStatus() const { return _status; }

        /**
         * @brief Встановлює обробник, який буде викликано після отримання пакету даних від сервера.
         *
         * @param data_handler Обробник події отримання даних.
         * @param arg Аргумент, який будуе передано обробнику.
         */
        void onData(const ServerDataHandler_t data_handler, void *arg);

        /**
         * @brief Встановлює обробник, який буде викликано після встановлення з'єднання з сервером.
         *
         * @param conn_handler Обробник події встановлення з'єднання з сервером.
         * @param arg Аргумент, який будуе передано обробнику.
         */
        void onConnect(const ServerConnectedHandler_t conn_handler, void *arg);

        /**
         * @brief Встановлює обробник, який буде викликано після втрати з'єднання з сервером.
         *
         * @param disconn_handler Обробник події втрати з'єднання з сервером.
         * @param arg Аргумент, який будуе передано обробнику.
         */
        void onDisconnect(ServerDisconnHandler_t disconn_handler, void *arg);

    protected:
        void sendHandshake();
        void sendName();
        //
        void handlePacket(UdpPacket *packet);
        static void packetHandlerTask(void *arg);
        //
        static void onPacket(void *arg, AsyncUDPPacket &packet);
        //
        void handleCheckConnect();
        static void checkConnectTask(void *arg);
        //
        void handleHandshake(const UdpPacket *packet);
        void handleNameConfirm(const UdpPacket *packet);
        void handlePing();
        void handleBusy();
        //
        void callDataHandler(UdpPacket *packet);
        void callConnectHandler();
        void callDisconnHandler();

    protected:
        AsyncUDP _client;

        IPAddress _server_ip;

        ServerConnectedHandler_t _server_connected_handler{nullptr};
        ServerDisconnHandler_t _server_disconn_handler{nullptr};
        ServerDataHandler_t _server_data_handler{nullptr};

        String _name;
        String _server_id;

        SemaphoreHandle_t _udp_mutex{nullptr};

        QueueHandle_t _packet_queue;

        TaskHandle_t _check_task_handler{nullptr};
        TaskHandle_t _packet_task_handler{nullptr};

        void *_server_connected_arg{nullptr};
        void *_server_disconn_arg{nullptr};
        void *_server_data_arg{nullptr};

        unsigned long _last_act_time{0};

        const uint16_t SERVER_PORT = 777;
        const uint16_t CLIENT_PORT = 777;
        const uint16_t PACKET_QUEUE_SIZE = 6;

        ClientStatus _status{STATUS_DISCONNECTED};

        bool _is_freed{true};
    };
} // namespace meow
