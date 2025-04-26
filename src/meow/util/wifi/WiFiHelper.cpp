#include "WiFiHelper.h"

const char STR_WIFI_BUSY[] = "WiFi-модуль зайнятий";

namespace meow
{
    WiFiHelper *WiFiHelper::_instance;

    WiFiHelper::WiFiHelper()
    {
        _instance = this;
    }

    bool WiFiHelper::tryConnectTo(String &ssid, String &pwd, uint8_t wifi_chan, bool autoreconnect)
    {
        if (_scan_working || _connect_working)
        {
            log_e("%s", STR_WIFI_BUSY);
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

        _connect_working = true;
        return true;
    }

    bool WiFiHelper::createAP(String &ssid, String &pwd, uint8_t max_connection, uint8_t wifi_chan, bool is_hidden)
    {
        if (_scan_working || _connect_working)
        {
            log_e("%s", STR_WIFI_BUSY);
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

    void WiFiHelper::setConnectDoneHandler(WiFiConnectDoneHandler handler, void *arg)
    {
        _connDoneHandler = handler;
        _connDoneHandlerArg = arg;
    }

    void WiFiHelper::setScanDoneHandler(WiFiScanDoneHandler handler, void *arg)
    {
        _scanDoneHandler = handler;
        _scanDoneHandlerArg = arg;
    }

    bool WiFiHelper::startScan()
    {
        if (_scan_working || _connect_working)
        {
            log_e("%s", STR_WIFI_BUSY);
            return false;
        }

        if (!isEnabled() && !enable())
        {
            return false;
        }
        else
        {
            if (isConnected())
                disconnect();

            if (WiFi.getMode() != WIFI_MODE_STA)
                enable();
        }

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
            _scan_working = true;
        }

        return true;
    }

    void WiFiHelper::getScanResult(std::vector<String> &out_vector) const
    {
        out_vector.clear();

        if (_scan_working || _connect_working)
        {
            log_e("%s", STR_WIFI_BUSY);
            return;
        }

        int16_t scan_result = WiFi.scanComplete();

        out_vector.reserve(scan_result);

        for (uint16_t i = 0; i < scan_result; ++i)
            out_vector.emplace_back(WiFi.SSID(i));

        WiFi.scanDelete();
    }

    void WiFiHelper::setWiFiPower(WiFiPowerLevel power_lvl)
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

    bool WiFiHelper::isConnected() const
    {
        return WiFi.status() == WL_CONNECTED;
    }

    bool WiFiHelper::isApEnabled() const
    {
        return WiFi.getMode() == WIFI_AP;
    }

    String WiFiHelper::getSsidName() const
    {
        if (isConnected())
            return WiFi.SSID();
        else
            return emptyString;
    }

    void WiFiHelper::disconnect()
    {
        _connDoneHandler = nullptr;
        _scanDoneHandler = nullptr;
        WiFi.disconnect();
    }

    bool WiFiHelper::isEnabled() const
    {
        return WiFi.getMode() != WIFI_MODE_NULL;
    }

    bool WiFiHelper::enable()
    {
        bool result = WiFi.mode(WIFI_MODE_STA);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        if (!result)
            log_e("Помилка увімкнення WiFi модуля");
        return result;
    }

    void WiFiHelper::disable()
    {
        disconnect();
        WiFi.mode(WIFI_OFF);
    }

    String WiFiHelper::getLocalIP() const
    {
        if (isConnected())
            return WiFi.localIP().toString();
        else
            return emptyString;
    }

    String WiFiHelper::getAPIP() const
    {
        if (isApEnabled())
            return WiFi.softAPIP().toString();
        else
            return emptyString;
    }

    void WiFiHelper::callConnDoneHandler()
    {
        log_i("WiFi.status: %d", WiFi.status());

        if (_connDoneHandler)
            _connDoneHandler(_connDoneHandlerArg, WiFi.status());
    }

    void WiFiHelper::callScanDoneHandler()
    {
        if (_scanDoneHandler)
            _scanDoneHandler(_scanDoneHandlerArg);
        else
            WiFi.scanDelete();
    }

    void WiFiHelper::onEvent(WiFiEvent_t event)
    {
        switch (event)
        {
        case ARDUINO_EVENT_WIFI_SCAN_DONE:
            WiFi.removeEvent(onEvent, ARDUINO_EVENT_WIFI_SCAN_DONE);
            _instance->_scan_working = false;
            _instance->callScanDoneHandler();
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
            _instance->_connect_working = false;
            _instance->callConnDoneHandler();
            break;
        default:
            log_e("Unknown wifi event: %u", event);
            break;
        }
    }
}
