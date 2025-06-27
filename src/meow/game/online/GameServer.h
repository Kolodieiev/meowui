#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <AsyncUDP.h>
#include <unordered_map>

#include "UdpPacket.h"
#include "ClientWrapper.h"

namespace meow
{
    class GameServer;

    /**
     * @brief Тип функції-обробника результату, яку буде надано сервером разом із запитом на авторизацію нового клієнта.
     *
     */
    typedef std::function<void(const ClientWrapper *client_wrap, bool result, GameServer *server_ptr)> ConfirmResultHandler_t;

    /**
     * @brief Тип обробника, який може бути викликано сервером у разі отримання нового запиту на авторизацію від клієнта.
     *
     */
    typedef std::function<void(const ClientWrapper *client_wrap, ConfirmResultHandler_t result_handler, void *arg)> ClientConfirmHandler_t;

    /**
     * @brief Тип обробника, який може бути викликано сервером у разі втрати з'єднання з одним із клієнтів.
     *
     */
    typedef std::function<void(const ClientWrapper *client_wrap, void *arg)> ClientDisconnHandler_t;

    /**
     * @brief Тип обробника, який може бути викликано сервером у разі отримання пакету даних від одного із клієнтів.
     * Не потрібно видаляти ClientWrapper* та UdpPacket*, ними керує сервер самостійно.
     *
     */
    typedef std::function<void(const ClientWrapper *client_wrap, const UdpPacket *packet, void *arg)> ClientDataHandler_t;

    class GameServer
    {

    public:
        GameServer();
        ~GameServer();

        /**
         * @brief Встановлє ідентифікатор сервера.
         *
         * @param id Ідентифікатор сервера.
         */
        void setServerID(const char *id) { _server_id = id; };

        /**
         * @brief Запускає сервер з вказаними параметрами.
         * Самостійно вмикає WiFi модуль, якщо is_local == true.
         * Інакше очікує, що з'єднання з точкою доступу вже встановлено.
         *
         * @param server_name Ім'я сервера, яке буде встановлено в якості імені точки доступу.
         * @param pwd Пароль точки доступу.
         * @param is_local Встановлює прапор, який вказує, чи буде сервер запущено на власній точці доступу, чи в мережі іншого маршрутизатораю
         * @param max_connection Максимальна кількість клієнтів.
         * @param wifi_chan Канал WiFi.
         * @return true - Якщо сервер успішно запущено.
         * @return false - Інакше.
         */
        bool begin(const char *server_name, const char *pwd, bool is_local = true, uint8_t max_connection = 1, uint8_t wifi_chan = 6);

        /**
         * @brief Скидає всі обробники подій, зупиняє сервер, звільняє ресурси та вимикає WiFi модуль.
         *
         */
        void stop();

        /**
         * @brief Вмикає можливість авторизації клієнтів, якщо на сервері доступні вільні слоти.
         *
         */
        void open();

        /**
         * @brief Вимикає можливість авторизації нових клієнтів.
         *
         */
        void close();

        /**
         * @brief Повертає значення прапору, який вказує на поточний стан можливості авторизації клієнтів на сервері.
         *
         * @return true - Якщо авторизація відкрита.
         * @return false - Інакше.
         */
        bool isOpen() const { return _is_open; }

        /**
         * @brief Повертає значення, яке вказує чи заповнено усі слоти сервера.
         *
         * @return true - Якщо усі слоти заповнено клієнтами.
         * @return false - Якщо ще лишаються вільні слоти.
         */
        bool isFull() const { return _max_connection == _cur_clients_size; }

        /**
         * @brief Видаляє клієнта з сервера за вказівним на його локальну обгортку.
         *
         * @param cl_wrap Вказівник на обгортку клієнта.
         */
        void removeClient(const ClientWrapper *cl_wrap);

        /**
         * @brief Видаляє клієнта з сервера за його ім'ям.
         *
         * @param client_name Рядок, що містить ім'я клієнта.
         */
        void removeClient(const char *client_name);

        /**
         * @brief Видаляє клієнта з сервера за його віддаленою ip-адресою.
         *
         * @param remote_ip Віддалена ip-адреса клієнта.
         */
        void removeClient(IPAddress remote_ip);

        /**
         * @brief Надсилає пакет усім підключеним клієнтам.
         *
         * @param packet Пакет, що буде надіслано усім клієнтам.
         */
        void sendBroadcast(UdpPacket &packet);

        /**
         * @brief Формує та надсилає пакет усім підключеним клієнтам.
         *
         * @param type Тип пакета.
         * @param data Дані, що будуть додані до пакета.
         * @param data_size Розмір даних.
         */
        void sendBroadcast(UdpPacket::PacketType type, const void *data, size_t data_size);

        /**
         * @brief Надсилає пакет на віддалену ip-адресу, що міститься в ClientWrapper *.
         *
         * @param cl_wrap Вказівник на обгортку клієнта.
         * @param packet Пакет, що буде надіслано клієнту.
         */
        void sendPacket(const ClientWrapper *cl_wrap, const UdpPacket &packet);

        /**
         * @brief Надсилає пакет на віддалену ip-адресу.
         *
         * @param remote_ip Віддалена ip-адреса клієнта.
         * @param packet Пакет, що буде надіслано клієнту.
         */
        void sendPacket(const IPAddress remote_ip, const UdpPacket &packet);

        /**
         * @brief Формує та надсилає пакет на віддалену ip-адресу.
         *
         * @param remote_ip Віддалена ip-адреса клієнта.
         * @param type Тип пакета.
         * @param data Дані, що будуть додані до пакета.
         * @param data_size Розмір даних.
         */
        void send(IPAddress remote_ip, UdpPacket::PacketType type, const void *data, size_t data_size);

        /**
         * @brief Встановлює обробник, який буде викликано коли з'явиться новий запит на авторизацію клієнта.
         *
         * @param handler Обробник, що буде викликано у разі настання події.
         * @param arg Аргумент, який буде передано обробнику.
         */
        void onConfirmation(ClientConfirmHandler_t handler, void *arg);

        /**
         * @brief Встановлює обробник, який буде викликано після втрати з'єднання з будь-яким із авторизованих клієнтів.
         *
         * @param handler Обробник, що буде викликано у разі настання події.
         * @param arg Аргумент, який буде передано обробнику.
         */
        void onDisconnect(ClientDisconnHandler_t handler, void *arg);

        /**
         * @brief Встановлює обробник, який буде викликано після отримання пакету даних від будь-якого із авторизованих клієнтів.
         *
         * @param handler Обробник, що буде викликано у разі настання події.
         * @param arg Аргумент, який буде передано обробнику.
         */
        void onData(ClientDataHandler_t handler, void *arg);

        /**
         * @brief Повертає вказівник на список клієнтів.
         * Під час взаємодії зі списком, асинхронність не забезпечується.
         *
         * @return const std::unordered_map<uint32_t, ClientWrapper *>*
         */
        const std::unordered_map<uint32_t, ClientWrapper *> *getClients() const { return &_clients; }

        /**
         * @brief Повертає локальну ip-адресу сервера.
         *
         * @return const char*
         */
        const char *getServerIP() const { return _server_ip.c_str(); }

        /**
         * @brief Повертає ім'я сервера.
         *
         * @return const char*
         */
        const char *getName() const { return _server_name.c_str(); }

    protected:
        ClientWrapper *findClient(const IPAddress remote_ip) const;
        ClientWrapper *findClient(const ClientWrapper *cl_wrap) const;
        ClientWrapper *findClient(const char *name) const;
        //
        void handlePacket(UdpPacket *packet);
        static void packetHandlerTask(void *arg);
        //
        static void onPacket(void *arg, AsyncUDPPacket &packet);
        //
        void handleHandshake(const UdpPacket *packet);
        void handleName(ClientWrapper *cl_wrap, const UdpPacket *packet);
        void handleData(ClientWrapper *cl_wrap, UdpPacket *packet);
        //
        void sendNameRespMsg(const ClientWrapper *cl_wrap, bool result);
        void sendBusyMsg(const ClientWrapper *cl_wrap);
        //
        void callDisconnHandler(const ClientWrapper *cl_wrap);
        void callClientConfirmHandler(const ClientWrapper *cl_wrap, ConfirmResultHandler_t result_handler);
        //
        void handlePingClient();
        static void pingClientTask(void *arg);
        //
        void handleNameConfirm(const ClientWrapper *cl_wrap, bool result);
        static void onConfirmationResult(const ClientWrapper *cl_wrap, bool result, GameServer *server_ptr);
        //
    protected:
        AsyncUDP _server;

        std::unordered_map<uint32_t, ClientWrapper *> _clients;

        ClientConfirmHandler_t _client_confirm_handler{nullptr};
        ClientDisconnHandler_t _client_disconn_handler{nullptr};
        ClientDataHandler_t _client_data_handler{nullptr};

        String _server_name;
        String _server_id;
        String _server_ip;

        TaskHandle_t _ping_task_handler{nullptr};
        TaskHandle_t _packet_task_handler{nullptr};

        SemaphoreHandle_t _client_mutex{nullptr};
        SemaphoreHandle_t _udp_mutex{nullptr};

        QueueHandle_t _packet_queue{nullptr};

        void *_client_confirm_arg{nullptr};
        void *_client_disconn_arg{nullptr};
        void *_client_data_arg{nullptr};

        const uint16_t SERVER_PORT = 777;
        const uint16_t PACKET_QUEUE_SIZE = 30;

        uint8_t _max_connection{1};
        uint8_t _cur_clients_size{0};

        bool _is_freed{true};
        bool _is_open{false};
        bool _is_busy{false};
    };
}