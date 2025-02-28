#pragma GCC optimize("O3")
#include "LRE220T.h"
#include <HardwareSerial.h>
#include "modules_setup/lora/LRE220.h"
#include "meow/util/crypto/aes256.h"

namespace meow
{
    bool LRE220T::begin()
    {
        log_i("Старт ініціалізації");
        pinMode(PIN_LRE220_M0, OUTPUT);
        pinMode(PIN_LRE220_M1, OUTPUT);
        pinMode(PIN_LRE220_AUX, INPUT);

        setMode(MODE_NORMAL);
        setMode(MODE_CONFIG);

        if (readRegisters())
        {
            setMode(MODE_NORMAL);

            log_i("LoRa ініціалізовано");
            _is_inited = true;
            return true;
        }

        log_e("Помилка ініціалізації");
        _is_inited = false;
        return false;
    }

    void LRE220T::end()
    {
        setMode(MODE_CONFIG);
        Serial1.end();
        _is_inited = false;
    }

    bool LRE220T::writeSettings()
    {
        if (!_is_inited)
            return false;

        setMode(MODE_NORMAL);
        setMode(MODE_CONFIG);

        setup();

        if (!writeRegisters())
            return false;

        setMode(MODE_NORMAL);

        log_i("Налаштування збережено");
        return true;
    }

    void LRE220T::warmUp()
    {
        if (!_is_inited)
            return;

        waitBusy();
        uint8_t buff = 0;
        Serial1.write(&buff, 1);
    }

    void LRE220T::writePacket(const uint8_t *buff)
    {
        if (!_is_inited)
            return;

        if (_has_encrypt)
        {
            uint8_t encrypted_packet[_settings.pack_byte_len];

            // #define IV_SIZE 12
            // #define TAG_SIZE 16 //TODO Розмір корисних даних повинен бути меншим

            if (aes256Encrypt(_settings.aes_key, buff, _settings.pack_byte_len, encrypted_packet))
            {
                waitBusy();
                Serial1.write(encrypted_packet, _settings.pack_byte_len);
            }
        }
        else
        {
            waitBusy();
            Serial1.write(buff, _settings.pack_byte_len);
        }
    }

    bool LRE220T::readPacket(uint8_t *out_buff)
    {
        uint8_t data_counter = 0;

        while (dataAvailable())
            out_buff[data_counter++] = Serial1.read();

        if (_has_encrypt)
        {
            // TODO decrypt

            return false;
        }

        return true;
    }

    uint8_t LRE220T::dataAvailable()
    {
        return Serial1.available();
    }

    void LRE220T::flushBuffer()
    {
        while (Serial1.available())
            Serial1.read();
    }

    void LRE220T::setMode(Mode mode)
    {
        waitBusy();

        if (mode == MODE_NORMAL)
        {
            Serial1.end();
            Serial1.begin(NORMAL_BAUD_RATE, SERIAL_8N1, PIN_LRE220_TX, PIN_LRE220_RX);

            digitalWrite(PIN_LRE220_M0, LOW);
            digitalWrite(PIN_LRE220_M1, HIGH);
            delay(20);
            digitalWrite(PIN_LRE220_M0, LOW);
            digitalWrite(PIN_LRE220_M1, LOW);
            delay(80);
        }
        else if (mode == MODE_CONFIG)
        {
            Serial1.end();
            Serial1.begin(CONFIG_BAUD_RATE, SERIAL_8N1, PIN_LRE220_TX, PIN_LRE220_RX);

            digitalWrite(PIN_LRE220_M0, HIGH);
            digitalWrite(PIN_LRE220_M1, HIGH);
            delay(100);
        }
        else
        {
            log_e("Unknown mode");
            esp_restart();
        }
    }

    bool LRE220T::isBusy()
    {
        return digitalRead(PIN_LRE220_AUX) != HIGH;
    }

    void LRE220T::waitBusy()
    {
        while (isBusy())
        {
        }
    }

    void LRE220T::setup()
    {
        // uint16_t value = 0x1234;
        // _reg_0->addr_high = (value >> 8);
        // _reg_1->addr_low = value & 0xFF;

        _reg_0->addr_high = 0;
        _reg_1->addr_low = 0;
        //
        _reg_2->serial_rate = 7;
        _reg_2->pairity_bit = 0;
        _reg_2->air_data_rate = 2;
        //
        _reg_3->packet_len = _settings.pack_bit_len;
        _reg_3->rssi_amb_en = 0;
        _reg_3->RESERVED = 0;
        _reg_3->transmit_power = _settings.transmit_power;
        //
        _reg_4->channel = _settings.channel;
        //
        _reg_5->rssi_byte_en = 0;
        _reg_5->transmit_meth = 0,
        _reg_5->RESERVED_1 = 0;
        _reg_5->lbt_en = 0;
        _reg_5->RESERVED_2 = 0;
        _reg_5->wor_cycle = 7;
        //
        _reg_6->crypt_key_h = 0;
        _reg_7->crypt_key_l = 0;
    }

    bool LRE220T::readRegisters()
    {
        log_i("Read registers");
        waitBusy();

        uint8_t cmd[] = {193, 0, sizeof(_shadow_registers)};
        Serial1.write(cmd, sizeof(cmd));

        if (!waitResponce(sizeof(cmd) + sizeof(_shadow_registers)))
            return false;

        Serial1.readBytes(cmd, sizeof(cmd));
        Serial1.readBytes(_shadow_registers, sizeof(_shadow_registers));

        return true;
    }

    bool LRE220T::writeRegisters()
    {
        log_i("Write registers");

        waitBusy();

        uint8_t cmd[] = {192, 0, sizeof(_shadow_registers)};
        uint8_t temp_cmd[sizeof(cmd) + sizeof(_shadow_registers)];
        memcpy(temp_cmd, cmd, sizeof(cmd));
        memcpy(temp_cmd + sizeof(cmd), _shadow_registers, sizeof(_shadow_registers));

        // log_i("");
        // for (int i = 3; i < sizeof(temp_cmd); i++)
        // {
        //     log_i("%02X ", temp_cmd[i]);
        // }
        // log_i("");

        Serial1.write(temp_cmd, sizeof(temp_cmd));

        if (!waitResponce(sizeof(temp_cmd)))
            return false;

        flushBuffer();

        return true;
    }

    bool LRE220T::waitResponce(size_t resp_size)
    {
        unsigned long start_time = millis();

        while (Serial1.available() != resp_size)
        {
            if (millis() - start_time > 250)
            {
                log_e("LoRa не відповідає або невірна команда");
                return false;
            }
        }

        return true;
    }

    void LRE220T::setPacketLen(PacketLen len)
    {
        _settings.pack_bit_len = len;

        if (_settings.pack_bit_len == PACKET_LEN_200)
            _settings.pack_byte_len = 200;
        else if (_settings.pack_bit_len == PACKET_LEN_128)
            _settings.pack_byte_len = 128;
        else if (_settings.pack_bit_len == PACKET_LEN_64)
            _settings.pack_byte_len = 64;
        else
            _settings.pack_byte_len = 32;
    }

    bool LRE220T::importSettings(char *file_name)
    {
        return false; // TODO
    }

    bool LRE220T::exportSettings(char *file_name)
    {
        return false; // TODO
    }

    void LRE220T::createNewKeys()
    {
        // TODO
    }

} // namespace meowui
