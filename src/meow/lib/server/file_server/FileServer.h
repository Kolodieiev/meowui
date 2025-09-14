#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include <WebServer.h>

namespace meow
{
    class FileServer
    {

    public:
        enum ServerMode : uint8_t
        {
            SERVER_MODE_RECEIVE = 0,
            SERVER_MODE_SEND
        };

        ~FileServer();

        /**
         * @brief Запускає файловий сервер з раніше налаштованими параметрами.
         * Якщо встановлено з'єднання з маршрутизатором, сервер буде запущено на виділеному IP.
         * Якщо з'єднання не встановлено раніше і його не вдалося встановити, буде створено точку доступу із заданими налаштуваннями.
         *
         * @param server_dir І'мя каталогу, для якого буде запущено файловий сервер.
         * @param mode Режим, в якому буде запущено файловий сервер. Може мати значення SERVER_MODE_SEND/SERVER_MODE_RECEIVE.
         * @return true - Якщо файловий сервер успішно запущено.
         * @return false - Інакше.
         */
        bool begin(const char *server_dir, ServerMode mode);

        /**
         * @brief Зупиняє файловий сервер. Та, якщо було запущено точку доступу, відключає WiFi-модуль.
         *
         */
        void stop();

        /**
         * @brief Повертає стан прапору, який вказує на те, чи працює зараз файловий сервер.
         *
         * @return true - Якщо сервер працює.
         * @return false - Інакше.
         */
        bool isWorking() const { return _is_working; }

        /**
         * @brief Встановлює SSID до якого буде виконано спробу підключення,
         * або з яким буде створено точку доступу, якщо підключення не вдалося.
         *
         * @param ssid
         */
        void setSSID(const char *ssid) { _ssid = ssid; }

        /**
         * @brief Встановлює пароль, який буде використано при підключенні до маршрутизатора,
         * або який буде використано для точки доступу, якщо підключення не вдалося.
         *
         * @param pwd
         */
        void setPWD(const char *pwd) { _pwd = pwd; }

        /**
         * @brief Повертає виділену IP-адресу сервера на маршрутизаторі або IP-адресу сервера на точці доступу.
         *
         * @return String - IP-адреса сервера, якщо сервер працює. Порожній рядок - інакше.
         */
        String getAddress() const;

        /**
         * @brief Повертає значення режиму роботи, яке налаштовано для сервера.
         *
         * @return ServerMode
         */
        ServerMode getServerMode() const { return _server_mode; }

    private:
        static void startWebServer(void *params);
        void fileServerTask(void *params);

        void handleReceive();
        void handleSend();
        void handleFile();
        void handle404();

    private:
        String _server_ip;
        String _server_dir;
        String _ssid;
        String _pwd;

        FILE *in_file{nullptr};
        WebServer *_server = nullptr;

        unsigned long _last_delay_ts = 0;

        ServerMode _server_mode{SERVER_MODE_RECEIVE};
        //
        bool _must_work = false;
        bool _is_working{false};
    };
}