#pragma once
#include <Arduino.h>
#include <WiFi.h>

namespace meow
{
    typedef std::function<void(void *arg, wl_status_t conn_status)> WiFiConnectHandler;

    class WiFiConnector
    {
    public:
        enum WiFiPowerLevel : uint8_t
        {
            WIFI_POWER_MIN = 0,
            WIFI_POWER_MEDIUM,
            WIFI_POWER_MAX,
        };

        WiFiConnector();

        /**
         * @brief Запускає асинхронну спробу підключення до точки доступу із заданими параметрами.
         *
         * @param ssid Ім'я точки доступу.
         * @param pwd Пароль точки доступу.
         * @param wifi_chan Канал WiFi.
         * @param autoreconnect Прапор, що вказує, чи потрібно виконувати автоматичну повторну спробу підключення
         * до точки доступу у разі невдачі або втрати з'єднання.
         * @return true - Якщо запуск спроби підключення виконано успішно.
         * @return false - Якщо не вдалося розпочати підключення.
         */
        bool tryConnectTo(String ssid, String pwd, int32_t wifi_chan = 0, bool autoreconnect = false);

        /**
         * @brief Встановлює обробник події, який буде викликано після завершення спроби підключення до точки доступу.
         *
         * @param handler Асинхронний обробник події завершення спроби підключення до точки доступу.
         * @param args Аргументи, що будуть передані обробнику.
         */
        void setOnDoneHandler(WiFiConnectHandler handler, void *arg);

        /**
         * @brief Встановлює потужність модуля WiFi.
         *
         * @param power_lvl Значення перечислення потужності модуля.
         * WIFI_POWER_MIN == 5 dBm.
         * WIFI_POWER_MEDIUM == 15 dBm.
         * WIFI_POWER_MAX == 19.5 dBm.
         */
        void setWiFiPower(WiFiPowerLevel power_lvl);

        /**
         * @brief Відключається від точки доступу, якщо було встановлено з'єднання, та вимикає модуль WiFi.
         *
         */
        void disconnect();

    private:
        void *_onConnectHandlerArg{nullptr};
        WiFiConnectHandler _onConnectHandler{nullptr};

        void callOnDoneHandler();

        static WiFiConnector *_instance;
        static void onEvent(WiFiEvent_t event);
    };
}