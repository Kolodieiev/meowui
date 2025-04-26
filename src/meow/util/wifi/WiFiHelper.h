#pragma once
#include <Arduino.h>
#include <WiFi.h>

#include <vector>

namespace meow
{
    typedef std::function<void(void *arg, wl_status_t conn_status)> WiFiConnectDoneHandler;
    typedef std::function<void(void *arg)> WiFiScanDoneHandler;

    class WiFiHelper
    {
    public:
        enum WiFiPowerLevel : uint8_t
        {
            WIFI_POWER_MIN = 0,
            WIFI_POWER_MEDIUM,
            WIFI_POWER_MAX,
        };

        WiFiHelper();

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
        bool tryConnectTo(String &ssid, String &pwd, uint8_t wifi_chan = 1, bool autoreconnect = false);

        /**
         * @brief Створює точку доступу WiFi, до якої можуть приєднуватися WiFi-клієнти.
         *
         * @param ssid Ім'я точки доступу.
         * @param pwd Пароль точки доступу.
         * @param max_connection Максимальна кількість підключень (більше 0, але менше 10).
         * Рекомендується встановлювати менше 5 одночасних підключень.
         * @param wifi_chan Канал WiFi (менше 11).
         * @param is_hidden Якщо true - точку доступу буде створено прихованою.
         * @return true - Якщо точка доступу створена успішно.
         * @return false - Інакше.
         */
        bool createAP(String &ssid, String &pwd, uint8_t max_connection = 1, uint8_t wifi_chan = 1, bool is_hidden = false);

        /**
         * @brief Встановлює обробник події, який буде викликано після завершення спроби підключення до точки доступу.
         *
         * @param handler Асинхронний обробник події завершення спроби підключення до точки доступу.
         * @param args Аргументи, що будуть передані обробнику.
         */
        void setConnectDoneHandler(WiFiConnectDoneHandler handler, void *arg);

        /**
         * @brief Встановлює обробник події, який буде викликано після завершення сканування точок WiFi.
         *
         * @param handler Асинхронний обробник події завершення сканування.
         * @param args Аргументи, що будуть передані обробнику.
         */
        void setScanDoneHandler(WiFiScanDoneHandler handler, void *arg);

        /**
         * @brief Налаштовує WiFi модуль та запускає сканування точок доступу.
         *
         * @return true - Якщо сканування було запущено.
         * @return false - Інакше.
         */
        bool startScan();

        /**
         * @brief Наповнює вектор іменами всіх виявлених точок доступу.
         * Після формування вектора, результат сканування очищується.
         *
         */
        void getScanResult(std::vector<String> &out_vector) const;

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
         * @brief Повертає логічне значення, яке вказує на те, чи встановлено з'єднання з WiFi.
         *
         * @return true - Якщо з'єднання встановлено.
         * @return false - Інакше.
         */
        bool isConnected() const;

        /**
         * @brief Повертає логічне значення, яке вказує на те, чи активовано точку доступу на пристрої.
         *
         * @return true - Якщо точка доступу активна.
         * @return false - Інакше.
         */
        bool isApEnabled() const;

        /**
         * @brief Повертає назву WiFi-точки доступу, з якою встановлено з'єднання.
         *
         * @return String - SSID точки доступу, якщо з'єднання існує, або порожній рядок, якщо з'єднання відсутнє.
         */
        String getSsidName() const;

        /**
         * @brief Відключається від точки доступу, якщо було встановлено з'єднання.
         *
         */
        void disconnect();

        /**
         * @brief Повертає стан модуля WiFi.
         *
         * @return true - Якщо модуль WiFi увімкнено.
         * @return false - Інакше.
         */
        bool isEnabled() const;

        /**
         * @brief Подає сигнал увімкнення на модуль WiFi.
         *
         * @return true - Якщо модуль WiFi було успішно увімкнено.
         * @return false - Інакше.
         */
        bool enable();

        /**
         * @brief Відключається від точки доступу, якщо було встановлено з'єднання, та вимикає модуль WiFi.
         *
         */
        void disable();

        /**
         * @brief Повертає IP-адресу пристрою в локальній мережі.
         *
         * @return String - IP-адреса пристрою в локальній мережі, якщо є підключення до маршрутизатора.
         * Порожній рядок - інакше.
         */
        String getLocalIP() const;

        /**
         * @brief Повертає IP-адресу точки доступу. Зазвичай це "192.168.4.1".
         *
         * @return String - IP-адреса точки доступу, якщо вона активована.
         * Порожній рядок - інакше.
         */
        String getAPIP() const;

    private:
        bool _scan_working{false};
        bool _connect_working{false};

        void *_scanDoneHandlerArg{nullptr};
        WiFiScanDoneHandler _scanDoneHandler{nullptr};

        void *_connDoneHandlerArg{nullptr};
        WiFiConnectDoneHandler _connDoneHandler{nullptr};

        void callConnDoneHandler();
        void callScanDoneHandler();

        static WiFiHelper *_instance;
        static void onEvent(WiFiEvent_t event);
    };
}