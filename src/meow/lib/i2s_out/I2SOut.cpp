#pragma GCC optimize("O3")
#include "I2SOut.h"
#include "meowui_setup/i2s_setup.h"

namespace meow
{
    bool I2SOut::_is_inited{false};

    bool I2SOut::init(uint16_t sample_rate)
    {
        if (_is_inited)
        {
            log_e("Одночасно може бути активовано тільки один вихід i2s");
            esp_restart();
        }

        i2s_config_t m_i2s_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
            .sample_rate = sample_rate,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 8,
            .dma_buf_len = 1024,
            .use_apll = false,
            .tx_desc_auto_clear = true,
            .fixed_mclk = 0};

        i2s_pin_config_t m_pin_config = {
            .mck_io_num = -1,
            .bck_io_num = PIN_I2S_OUT_BCLK,
            .ws_io_num = PIN_I2S_OUT_LRC,
            .data_out_num = PIN_I2S_OUT_DOUT,
            .data_in_num = -1,
        };

        bool result = false;
        result |= i2s_driver_install(I2S_NUM_0, &m_i2s_config, 0, NULL);
        result |= i2s_zero_dma_buffer(I2S_NUM_0);
        result |= i2s_set_pin(I2S_NUM_0, &m_pin_config);

        if (result)
            log_e("Помилка ініціалізації I2SOut");
        else
            _is_inited = true;

        return !result;
    }

    size_t I2SOut::write(const int16_t *buffer, size_t buff_len, bool one_chann_buff)
    {
        if (!_is_inited)
        {
            log_e("I2SOut не ініціалізовано");
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

            i2s_write(I2S_NUM_0, buffer_copy, buff_len * 2 * sizeof(int16_t), &bytes_written, 0);
        }
        else
        {
            i2s_write(I2S_NUM_0, buffer, buff_len * sizeof(int16_t), &bytes_written, 0);
        }

        return bytes_written;
    }

    void I2SOut::deinit()
    {
        if (!_is_inited)
            return;

        i2s_driver_uninstall(I2S_NUM_0);
        _is_inited = false;
    }
}