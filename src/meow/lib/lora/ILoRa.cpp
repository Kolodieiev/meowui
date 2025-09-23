#include "ILoRa.h"
#define MAX_RESP_WAIT_TIME_MS 250UL
#define PACKET_SENDING_DELAY 3U
#define PACKET_RECEIVE_DELAY 5U
#define MAX_PACKET_LEN 58U
#define CONFIG_BAUD_RATE 9600UL
#define NORMAL_BAUD_RATE 115200UL

#define LORA_BUSY_TIMEOUT 1000UL

namespace meow
{
    ILoRa::ILoRa(AirDataRate min_data_rate, AirDataRate max_data_rate, uint8_t max_chann)
        : _min_air_data_rate{min_data_rate},
          _max_air_data_rate{max_data_rate},
          _max_chann{max_chann}
    {
    }

    ILoRa::~ILoRa()
    {
        end();
    }

    bool ILoRa::init()
    {
        log_i("Початок ініціалізації LoRa");
        pinMode(PIN_LR_M0, OUTPUT);
        pinMode(PIN_LR_M1, OUTPUT);
        pinMode(PIN_LR_AUX, INPUT);

        setMode(MODE_NORMAL);
        setMode(MODE_CONFIG);

        if (readRegisters())
        {
            log_i("LoRa ініціалізовано");
            _is_inited = true;
            return true;
        }

        log_e("Помилка ініціалізації");
        _is_inited = false;
        return false;
    }

    bool ILoRa::writeSettings(bool temporary)
    {
        if (!_is_inited)
            return false;

        setMode(MODE_NORMAL);
        flushBuffer();
        setMode(MODE_CONFIG);

        configShadowReg();

        if (!writeRegisters(temporary))
        {
            log_e("Помилка запису налаштуваннь");
            return false;
        }

        log_i("Налаштування збережено");
        return true;
    }

    void ILoRa::end()
    {
        setMode(MODE_CONFIG);
        Serial1.end();
        _is_inited = false;
    }

    void ILoRa::setPackLen(uint8_t len)
    {
        if (len == 0 || len > MAX_PACKET_LEN)
        {
            log_e("Некоректний розмір пакета: %u", len);
            esp_restart();
        }

        _packet_len = len;
    }

    void ILoRa::setMode(Mode mode)
    {
        waitWhileBusy();

        if (mode == MODE_NORMAL)
        {
            Serial1.end();
            Serial1.begin(NORMAL_BAUD_RATE, SERIAL_8N1, PIN_LR_TX, PIN_LR_RX);

            digitalWrite(PIN_LR_M0, LOW);
            digitalWrite(PIN_LR_M1, HIGH);
            waitWhileBusy();

            digitalWrite(PIN_LR_M0, LOW);
            digitalWrite(PIN_LR_M1, LOW);
            waitWhileBusy();

            _mode = MODE_NORMAL;
        }
        else if (mode == MODE_CONFIG)
        {
            Serial1.end();
            Serial1.begin(CONFIG_BAUD_RATE, SERIAL_8N1, PIN_LR_TX, PIN_LR_RX);

            digitalWrite(PIN_LR_M0, HIGH);
            digitalWrite(PIN_LR_M1, HIGH);
            waitWhileBusy();

            _mode = MODE_CONFIG;
        }
        else
        {
            log_e("Unknown mode");
            esp_restart();
        }
    }

    void ILoRa::setTransmitPower(TransmitPower power)
    {
        power > TR_POWER_LOW ? _transmit_power = TR_POWER_LOW : _transmit_power = power;
    }

    void ILoRa::setAirDataRate(AirDataRate rate)
    {
        if (rate < _min_air_data_rate)
            _air_data_rate = _min_air_data_rate;
        else if (rate > _max_air_data_rate)
            _air_data_rate = _max_air_data_rate;
        else
            _air_data_rate = rate;
    }

    void ILoRa::setChannel(uint8_t channel)
    {
        channel > _max_chann ? _channel = _max_chann : _channel = channel;
    }

    void ILoRa::waitWhileBusy()
    {
        unsigned long busy_ts = millis();

        while (isBusy() && millis() - busy_ts < LORA_BUSY_TIMEOUT)
        {
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

        vTaskDelay(2 / portTICK_PERIOD_MS);
    }

    bool ILoRa::waitResponce(size_t resp_size)
    {
        unsigned long start_time = millis();

        while (Serial1.available() != resp_size)
        {
            if (millis() - start_time > MAX_RESP_WAIT_TIME_MS)
            {
                log_e("LoRa не відповідає або невірна команда");
                return false;
            }

            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

        return true;
    }

    void ILoRa::writePacket(const uint8_t *buff)
    {
        if (!_is_inited)
            return;

        waitWhileBusy();
        Serial1.write(buff, _packet_len);
        vTaskDelay(PACKET_SENDING_DELAY / portTICK_PERIOD_MS);
    }

    bool ILoRa::readPack(uint8_t *out_buff)
    {
        uint8_t data_counter = 0;

        while (Serial1.available())
        {
            out_buff[data_counter++] = Serial1.read();
            if (data_counter > _packet_len && Serial1.available())
            {
                flushBuffer();
                return false;
            }
        }

        return data_counter == _packet_len;
    }

    bool ILoRa::packAvailable()
    {
        if (!isBusy())
            return false;

        int avail = Serial1.available();

        if (avail == 0)
            return false;

        if (avail != _packet_len)
        {
            flushBuffer();
            return false;
        }

        return true;
    }

    void ILoRa::flushBuffer()
    {
        while (Serial1.available())
            Serial1.read();
    }

    bool ILoRa::isBusy()
    {
        return digitalRead(PIN_LR_AUX) != HIGH;
    }
}
