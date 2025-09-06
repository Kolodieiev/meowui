#include "WiFiManager.h"

const char STR_ERR_WIFI_BUSY[] = "WiFi-модуль зайнятий";

namespace meow
{
    bool WiFiManager::tryConnectTo(const String &ssid, const String &pwd, uint8_t wifi_chan, bool autoreconnect)
    {
        if (_is_busy)
        {
            log_e("%s", STR_ERR_WIFI_BUSY);
            return false;
        }

        if (isConnected())
            disconnect();

        if (wifi_chan > 10)
            wifi_chan = 10;

        WiFi.onEvent(onEvent, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
        WiFi.onEvent(onEvent, ARDUINO_EVENT_WIFI_STA_GOT_IP);
        WiFi.setAutoReconnect(autoreconnect);
        WiFi.persistent(false);
        wl_status_t status = WiFi.begin(ssid, pwd, wifi_chan);

        if (status != WL_DISCONNECTED)
        {
            log_e("Помилка приєднання до: %s", ssid.c_str());
            WiFi.removeEvent(onEvent, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
            WiFi.removeEvent(onEvent, ARDUINO_EVENT_WIFI_STA_GOT_IP);
            return false;
        }

        _is_busy = true;
        return true;
    }

    bool WiFiManager::createAP(String &ssid, String &pwd, uint8_t max_connection, uint8_t wifi_chan, bool is_hidden)
    {
        if (_is_busy)
        {
            log_e("%s", STR_ERR_WIFI_BUSY);
            return false;
        }

        if (isConnected())
            disconnect();

        if (max_connection > 9)
            max_connection = 9;

        if (max_connection == 0)
            max_connection = 1;

        if (wifi_chan > 10)
            wifi_chan = 10;

        bool result = WiFi.softAP(ssid, pwd, wifi_chan, is_hidden, max_connection);
        delay(100);

        if (!result)
            log_e("Помилка створення точки доступу");

        return result;
    }

    void WiFiManager::setConnectDoneHandler(WiFiConnectDoneHandler handler, void *arg)
    {
        _connDoneHandler = handler;
        _connDoneHandlerArg = arg;
    }

    void WiFiManager::setScanDoneHandler(WiFiScanDoneHandler handler, void *arg)
    {
        _scanDoneHandler = handler;
        _scanDoneHandlerArg = arg;
    }

    bool WiFiManager::startScan()
    {
        if (_is_busy)
        {
            log_e("%s", STR_ERR_WIFI_BUSY);
            return false;
        }

        if (!isEnabled() && !enable())
            return false;
        else if (WiFi.getMode() != WIFI_MODE_STA)
            enable();

        WiFi.onEvent(onEvent, ARDUINO_EVENT_WIFI_SCAN_DONE);
        int16_t result_code = WiFi.scanNetworks(true);
        if (result_code == WIFI_SCAN_FAILED)
        {
            log_e("Помилка запуску сканера Wi-Fi");
            WiFi.removeEvent(onEvent, ARDUINO_EVENT_WIFI_SCAN_DONE);
            return false;
        }
        else
        {
            _is_busy = true;
        }

        return true;
    }

    void WiFiManager::getScanResult(std::vector<String> &out_vector)
    {
        out_vector.clear();

        if (_is_busy)
        {
            log_e("%s", STR_ERR_WIFI_BUSY);
            return;
        }

        int16_t scan_result = WiFi.scanComplete();

        if (scan_result == -1)
        {
            log_e("Scan not fin");
            return;
        }
        else if (scan_result == -2)
        {
            log_e("Scan not triggered");
            return;
        }

        out_vector.reserve(scan_result);

        for (uint16_t i = 0; i < scan_result; ++i)
            out_vector.emplace_back(WiFi.SSID(i));

        WiFi.scanDelete();
    }

    void WiFiManager::clearScanResult()
    {
        WiFi.scanDelete();
    }

    void WiFiManager::setWiFiPower(WiFiPowerLevel power_lvl)
    {
        switch (power_lvl)
        {
        case WIFI_POWER_MIN:
            WiFi.setTxPower(WIFI_POWER_5dBm);
            break;
        case WIFI_POWER_MEDIUM:
            WiFi.setTxPower(WIFI_POWER_15dBm);
            break;
        case WIFI_POWER_MAX:
            WiFi.setTxPower(WIFI_POWER_19_5dBm);
            break;
        }
    }

    bool WiFiManager::isConnected() const
    {
        return WiFi.status() == WL_CONNECTED;
    }

    bool WiFiManager::isApEnabled() const
    {
        return WiFi.getMode() & WIFI_AP;
    }

    String WiFiManager::getSSID() const
    {
        if (isConnected())
            return WiFi.SSID();
        else
            return emptyString;
    }

    void WiFiManager::disconnect()
    {
        _connDoneHandler = nullptr;
        _scanDoneHandler = nullptr;
        WiFi.disconnect();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    bool WiFiManager::isEnabled() const
    {
        return WiFi.getMode() != WIFI_MODE_NULL;
    }

    bool WiFiManager::enable()
    {
        bool result = WiFi.mode(WIFI_MODE_STA);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        if (!result)
            log_e("Помилка увімкнення WiFi модуля");
        return result;
    }

    void WiFiManager::disable()
    {
        disconnect();
        WiFi.mode(WIFI_OFF);
    }

    bool WiFiManager::toggle()
    {
        if (_is_busy)
        {
            log_e("Модуль зайнятий");
            return false;
        }

        if (isEnabled())
        {
            disable();
            return true;
        }
        else
        {
            return enable();
        }
    }

    String WiFiManager::getLocalIP() const
    {
        if (isConnected())
            return WiFi.localIP().toString();
        else
            return emptyString;
    }

    String WiFiManager::getAPIP() const
    {
        if (isApEnabled())
            return WiFi.softAPIP().toString();
        else
            return emptyString;
    }

    void WiFiManager::callConnDoneHandler()
    {
        log_i("WiFi.status: %d", WiFi.status());

        if (_connDoneHandler)
            _connDoneHandler(_connDoneHandlerArg, WiFi.status());
    }

    void WiFiManager::callScanDoneHandler()
    {
        if (_scanDoneHandler)
            _scanDoneHandler(_scanDoneHandlerArg);
        else
            WiFi.scanDelete();
    }

    void WiFiManager::onEvent(WiFiEvent_t event)
    {
        switch (event)
        {
        case ARDUINO_EVENT_WIFI_SCAN_DONE:
            WiFi.removeEvent(onEvent, ARDUINO_EVENT_WIFI_SCAN_DONE);
            _wifi._is_busy = false;
            _wifi.callScanDoneHandler();
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            WiFi.removeEvent(onEvent, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
            WiFi.removeEvent(onEvent, ARDUINO_EVENT_WIFI_STA_GOT_IP);

            if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP)
            {
                long unsigned got_ip_time = millis();
                while (millis() - got_ip_time < 2000 && WiFi.status() != WL_CONNECTED)
                    vTaskDelay(50 / portTICK_PERIOD_MS);
            }
            _wifi._is_busy = false;
            _wifi.callConnDoneHandler();
            break;
        default:
            log_e("Unknown wifi event: %u", event);
            break;
        }
    }

    WiFiManager _wifi;
}
