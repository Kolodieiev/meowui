#pragma GCC optimize("O3")
#include "I2SWriter.h"
#include "meowui_setup/i2s_setup.h"

namespace meow
{
    bool I2SWriter::init(i2s_port_t port, uint16_t sample_rate)
    {
        _port = port;

        i2s_config_t m_i2s_config = {};
        m_i2s_config.sample_rate = sample_rate;
        m_i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
        m_i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
        m_i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
        m_i2s_config.dma_buf_len = 1024;
        m_i2s_config.dma_buf_count = 8;
        m_i2s_config.use_apll = false;
        m_i2s_config.tx_desc_auto_clear = true;
        m_i2s_config.fixed_mclk = false;
        m_i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
        m_i2s_config.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S);

        i2s_pin_config_t m_pin_config = {};
        m_pin_config.bck_io_num = PIN_I2S_BCLK;
        m_pin_config.ws_io_num = PIN_I2S_LRC;
        m_pin_config.data_out_num = PIN_I2S_DOUT;
        m_pin_config.data_in_num = -1;
        m_pin_config.mck_io_num = -1;

        bool result = false;
        result |= i2s_driver_install(_port, &m_i2s_config, 0, NULL);
        result |= i2s_zero_dma_buffer(_port);
        result |= i2s_set_pin(_port, &m_pin_config);

        if (result)
            log_e("Помилка ініціалізації I2SWriter");
        else
            _is_inited = true;

        return !result;
    }

    size_t I2SWriter::write(const int16_t *buffer, size_t buff_len, bool one_chann_buff)
    {
        if (!_is_inited)
        {
            log_e("I2SWriter не ініціалізовано");
            esp_restart();
        }

        size_t bytes_written;

        if (one_chann_buff)
        {
            int16_t buffer_copy[buff_len * 2];
            int16_t *dst = buffer_copy;

            for (int i = 0; i < buff_len; ++i)
            {
                *dst++ = buffer[i];
                *dst++ = buffer[i];
            }

            i2s_write(_port, buffer_copy, buff_len * 2 * sizeof(int16_t), &bytes_written, 0);
        }
        else
        {
            i2s_write(_port, buffer, buff_len * sizeof(int16_t), &bytes_written, 0);
        }

        return bytes_written;
    }

    void I2SWriter::deinit()
    {
        if (!_is_inited)
            return;

        i2s_driver_uninstall(_port);

        _is_inited = false;
    }
}