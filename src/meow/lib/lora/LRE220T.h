#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>

namespace meow
{
    class LRE220T
    {
    public:
        enum TransmitPower : uint8_t
        {
            POWER_MAX = 0,
            POWER_HIGH,
            POWER_MID,
            POWER_LOW
        };

        enum PacketLen : uint8_t
        {
            PACKET_LEN_200 = 0,
            PACKET_LEN_128,
            PACKET_LEN_64,
            PACKET_LEN_32,
        };

        // ----------------------------------------------------------------
        //
        bool begin();
        void end();
        bool writeSettings();
        //
        void warmUp();
        void writePacket(const uint8_t *buff);
        bool readPacket(uint8_t *out_buff);
        uint8_t read();
        uint8_t dataAvailable();
        void flushBuffer();
        //
        void setChannel(uint8_t channel) { channel > 80 ? _settings.channel = 80 : _settings.channel = channel; }
        uint8_t getChannel() const { return _settings.channel; }
        //
        void setTransmitPower(TransmitPower power) { _settings.transmit_power = power; }
        TransmitPower getTransmitPower() const { return _settings.transmit_power; }
        //
        void setPacketLen(PacketLen len);
        PacketLen getPacketLen() const { return _settings.pack_bit_len; }
        //
        bool importSettings(char *file_name);
        bool exportSettings(char *file_name);
        //
        void setEncrypt(bool state) { _has_encrypt = state; }
        bool hasEncrypt() const { return _has_encrypt; }

        void createNewKeys();

    private:
        enum Mode : uint8_t
        {
            MODE_CONFIG,
            MODE_NORMAL
        };

        struct REG_0
        {
            uint8_t addr_high{0};
        };

        struct REG_1
        {
            uint8_t addr_low{0};
        };

        struct REG_2
        {
            uint8_t air_data_rate : 3;
            uint8_t pairity_bit : 2;
            uint8_t serial_rate : 3;
        };

        struct REG_3
        {
            TransmitPower transmit_power : 2;
            uint8_t RESERVED : 3;
            uint8_t rssi_amb_en : 1;
            uint8_t packet_len : 2;
        };

        struct REG_4
        {
            uint8_t channel{0};
        };

        struct REG_5
        {
            uint8_t wor_cycle : 3;
            uint8_t RESERVED_2 : 1;
            uint8_t lbt_en : 1;
            uint8_t RESERVED_1 : 1;
            uint8_t transmit_meth : 1;
            uint8_t rssi_byte_en : 1;
        };

        struct REG_6
        {
            uint8_t crypt_key_h{0};
        };

        struct REG_7
        {
            uint8_t crypt_key_l{0};
        };

        const uint32_t CONFIG_BAUD_RATE = 9600;
        const uint32_t NORMAL_BAUD_RATE = 115200;

        struct CommonSettings
        {
            TransmitPower transmit_power{POWER_MAX};
            PacketLen pack_bit_len{PACKET_LEN_64};
            uint8_t pack_byte_len{64};
            uint8_t channel{23};
            uint8_t aes_key[32];
        } _settings;

        bool _is_inited{false};
        bool _has_encrypt{true};
        //
        uint8_t _shadow_registers[8];
        REG_0 *_reg_0 = (REG_0 *)&_shadow_registers[0];
        REG_1 *_reg_1 = (REG_1 *)&_shadow_registers[1];
        REG_2 *_reg_2 = (REG_2 *)&_shadow_registers[2];
        REG_3 *_reg_3 = (REG_3 *)&_shadow_registers[3];
        REG_4 *_reg_4 = (REG_4 *)&_shadow_registers[4];
        REG_5 *_reg_5 = (REG_5 *)&_shadow_registers[5];
        REG_6 *_reg_6 = (REG_6 *)&_shadow_registers[6];
        REG_7 *_reg_7 = (REG_7 *)&_shadow_registers[7];
        //
        void setMode(Mode mode);

        bool isBusy();

        void setup();
        bool readRegisters();
        bool writeRegisters();
        bool waitResponce(size_t resp_size);
        void waitBusy();
    };
} // namespace meowui
