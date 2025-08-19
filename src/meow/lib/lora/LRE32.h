#pragma once
#include <Arduino.h>
#include "./ILoRa.h"

namespace meow
{
    class LRE32 : public ILoRa
    {
    public:
        LRE32();
        virtual ~LRE32() override {}

    protected:
        virtual void configShadowReg() final override;
        virtual bool writeRegisters(bool temporary) final override;
        virtual bool readRegisters() final override;

    private:
        struct REG_HEAD
        {
            uint8_t ctrl_cmd{0xC2};
        };

        struct REG_ADDRH
        {
            uint8_t addr_high{0};
        };

        struct REG_ADDRL
        {
            uint8_t addr_low{0};
        };

        struct REG_SPEED
        {
            AirDataRate air_data_rate : 3;
            uint8_t serial_rate : 3;
            uint8_t pairity_bit : 2; // Мусить бути 0.
        };

        struct REG_CHAN
        {
            uint8_t channel : 5;  // （410MHz+CHAN * 1MHz）, default 23
            uint8_t RESERVED : 3; // Мусить бути 0.
        };

        struct REG_OPTION
        {
            TransmitPower transmit_power : 2;
            bool en_err_correction : 1;
            uint8_t wake_up_time : 3; // Мусить бути 0.
            bool use_internal_io_drive : 1;
            bool en_fixed_transmit_mode : 1;
        };

    private:
        uint8_t _shadow_registers[6]{};

        REG_HEAD *_reg_head = reinterpret_cast<REG_HEAD *>(&_shadow_registers[0]);
        REG_ADDRH *_reg_addrh = reinterpret_cast<REG_ADDRH *>(&_shadow_registers[1]);
        REG_ADDRL *_reg_addrl = reinterpret_cast<REG_ADDRL *>(&_shadow_registers[2]);
        REG_SPEED *_reg_speed = reinterpret_cast<REG_SPEED *>(&_shadow_registers[3]);
        REG_CHAN *_reg_chan = reinterpret_cast<REG_CHAN *>(&_shadow_registers[4]);
        REG_OPTION *_reg_opt = reinterpret_cast<REG_OPTION *>(&_shadow_registers[5]);

        uint8_t _packet_len{0};
    };
}