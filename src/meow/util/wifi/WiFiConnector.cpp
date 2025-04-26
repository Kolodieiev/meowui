#include "WiFiConnector.h"

namespace meow
{
    WiFiConnector *WiFiConnector::_instance;

    WiFiConnector::WiFiConnector()
    {
        _instance = this;
    }

    bool WiFiConnector::tryConnectTo(String ssid, String pwd, int32_t wifi_chan, bool autoreconnect)
    {
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

        return true;
    }

    void WiFiConnector::setOnDoneHandler(WiFiConnectHandler handler, void *arg)
    {
        _onConnectHandler = handler;
        _onConnectHandlerArg = arg;
    }

    void WiFiConnector::setWiFiPower(WiFiPowerLevel power_lvl)
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

    void WiFiConnector::disconnect()
    {
        _onConnectHandler = nullptr;
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
    }

    void WiFiConnector::callOnDoneHandler()
    {
        log_i("WiFi.status: %d", WiFi.status());

        if (_onConnectHandler)
            _onConnectHandler(_onConnectHandlerArg, WiFi.status());
    }

    void WiFiConnector::onEvent(WiFiEvent_t event)
    {
        WiFi.removeEvent(onEvent, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
        WiFi.removeEvent(onEvent, ARDUINO_EVENT_WIFI_STA_GOT_IP);

        if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP)
        {
            long unsigned got_ip_time = millis();
            while (millis() - got_ip_time < 2000 && WiFi.status() != WL_CONNECTED)
                vTaskDelay(50 / portTICK_PERIOD_MS);
        }

        _instance->callOnDoneHandler();
    }
}
