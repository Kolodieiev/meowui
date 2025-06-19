#include "LRE32.h"
#include <HardwareSerial.h>

#define LRE32_MIN_DATA_RATE ILoRa::AIR_DATA_RATE_0_3K
#define LRE32_MAX_DATA_RATE ILoRa::AIR_DATA_RATE_19_2K
#define LRE32_MAX_CHAN 32

namespace meow
{
    LRE32::LRE32() : ILoRa(LRE32_MIN_DATA_RATE, LRE32_MAX_DATA_RATE, LRE32_MAX_CHAN)
    {
    }

    void LRE32::configShadowReg()
    {
        _reg_addrh->addr_high = 0;
        _reg_addrl->addr_low = 0;
        //
        _reg_speed->air_data_rate = getAirDataRate();
        _reg_speed->serial_rate = 7; // 115200
        _reg_speed->pairity_bit = 0;
        //
        _reg_chan->channel = getChannel();
        _reg_chan->RESERVED = 0;
        //
        _reg_opt->transmit_power = getTransmitPower();
        _reg_opt->en_err_correction = true;
        _reg_opt->wake_up_time = 0;
        _reg_opt->use_internal_io_drive = true;
        _reg_opt->en_fixed_transmit_mode = false;
    }

    bool LRE32::writeRegisters(bool temporary)
    {
        if (temporary)
            _reg_head->ctrl_cmd = 0xC2;
        else
            _reg_head->ctrl_cmd = 0xC0;

        waitWhileBusy();

        // log_i("");
        // log_i("Write registers");
        // for (int i = 0; i < sizeof(_shadow_registers); ++i)
        // {
        //     log_i("%02X ", _shadow_registers[i]);
        // }
        // log_i("");

        Serial1.write(_shadow_registers, sizeof(_shadow_registers));

        if (!waitResponce(sizeof(_shadow_registers)))
        {
            log_e("Помилка запису регістрів LoRa");
            return false;
        }

        flushBuffer();

        return true;
    }

    bool LRE32::readRegisters()
    {
        waitWhileBusy();

        uint8_t cmd[] = {0xC1, 0xC1, 0xC1};

        Serial1.write(cmd, sizeof(cmd));

        if (!waitResponce(sizeof(_shadow_registers)))
        {
            log_e("Помилка читання регістрів LoRa");
            return false;
        }

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
}
