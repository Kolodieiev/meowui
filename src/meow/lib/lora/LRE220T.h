#pragma once
#include <Arduino.h>
#include "./ILoRa.h"

namespace meow
{
    class LRE220T : public ILoRa
    {
    public:
        LRE220T();
        virtual ~LRE220T() override {}

    protected:
        virtual void configShadowReg() override;
        virtual bool writeRegisters(bool temporary) override;
        virtual bool readRegisters() override;

    private:
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

    private:
        uint8_t _shadow_registers[8]{};

        REG_0 *_reg_0 = reinterpret_cast<REG_0 *>(&_shadow_registers[0]);
        REG_1 *_reg_1 = reinterpret_cast<REG_1 *>(&_shadow_registers[1]);
        REG_2 *_reg_2 = reinterpret_cast<REG_2 *>(&_shadow_registers[2]);
        REG_3 *_reg_3 = reinterpret_cast<REG_3 *>(&_shadow_registers[3]);
        REG_4 *_reg_4 = reinterpret_cast<REG_4 *>(&_shadow_registers[4]);
        REG_5 *_reg_5 = reinterpret_cast<REG_5 *>(&_shadow_registers[5]);
        REG_6 *_reg_6 = reinterpret_cast<REG_6 *>(&_shadow_registers[6]);
        REG_7 *_reg_7 = reinterpret_cast<REG_7 *>(&_shadow_registers[7]);
    };
} // namespace meowui
