#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <driver/i2s.h>

namespace meow
{
    class I2SWriter
    {
    public:
        ~I2SWriter() { deinit(); }

        bool init(i2s_port_t port, uint16_t buff_size, uint16_t sample_rate = 16000);
        void deinit();
        size_t write(const void *buffer, bool one_chann_buff = true);

    private:
        bool _is_inited = false;
        i2s_port_t _port;
        uint16_t _buff_size;
    };
}