#include "LRE220T.h"
#include <HardwareSerial.h>

#define LRE220_MIN_DATA_RATE ILoRa::AIR_DATA_RATE_2_4K
#define LRE220_MAX_DATA_RATE ILoRa::AIR_DATA_RATE_62_5K
#define LRE220_MAX_CHAN 80
#define LRE220_MAX_PACK_LEN 64

namespace meow
{
    LRE220T::LRE220T() : ILoRa(LRE220_MIN_DATA_RATE, LRE220_MAX_DATA_RATE, LRE220_MAX_PACK_LEN, LRE220_MAX_CHAN)
    {
    }

    void LRE220T::configShadowReg()
    {
        _reg_0->addr_high = 0;
        _reg_1->addr_low = 0;
        //
        _reg_2->serial_rate = 7;
        _reg_2->pairity_bit = 0;
        _reg_2->air_data_rate = getAirDataRate();
        //
        _reg_3->packet_len = 2;
        _reg_3->rssi_amb_en = 0;
        _reg_3->RESERVED = 0;
        _reg_3->transmit_power = getTransmitPower();
        //
        _reg_4->channel = getChannel();
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

    bool LRE220T::writeRegisters(bool temporary)
    {
        waitWhileBusy();

        uint8_t cmd[3];

        if (temporary)
            cmd[0] = 0xC2;
        else
            cmd[0] = 0xC0;

        cmd[1] = 0;
        cmd[2] = sizeof(_shadow_registers);

        // log_i("");
        // log_i("Write registers");
        // for (int i = 0; i < sizeof(_shadow_registers); ++i)
        // {
        //     log_i("%02X ", _shadow_registers[i]);
        // }
        // log_i("");

        Serial1.write(cmd, sizeof(cmd));
        Serial1.write(_shadow_registers, sizeof(_shadow_registers));

        if (!waitResponce(sizeof(cmd) + sizeof(_shadow_registers)))
        {
            log_e("Помилка запису регістрів LoRa");
            return false;
        }

        flushBuffer();

        return true;
    }

    bool LRE220T::readRegisters()
    {
        waitWhileBusy();

        uint8_t cmd[]{0xC1, 0, sizeof(_shadow_registers)};
        Serial1.write(cmd, sizeof(cmd));

        if (!waitResponce(sizeof(cmd) + sizeof(_shadow_registers)))
        {
            log_e("Помилка читання регістрів LoRa");
            return false;
        }

        Serial1.readBytes(cmd, sizeof(cmd));
        Serial1.readBytes(_shadow_registers, sizeof(_shadow_registers));

        // log_i("");
        // log_i("Read registers");
        // for (int i = 0; i < sizeof(_shadow_registers); ++i)
        // {
        //     log_i("%02X ", _shadow_registers[i]);
        // }
        // log_i("");

        return true;
    }

} // namespace meowui
