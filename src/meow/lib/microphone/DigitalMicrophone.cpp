#pragma GCC optimize("O3")
#include "DigitalMicrophone.h"
#include "meowui_setup/i2s_setup.h"

namespace meow
{
    bool DigitalMicrophone::_is_inited{false};

    bool DigitalMicrophone::init(uint16_t sample_rate)
    {
        if (_is_inited)
        {
            log_e("Одночасно може бути активовано тільки один вхід i2s");
            esp_restart();
        }

        i2s_config_t i2s_mic_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
            .sample_rate = sample_rate,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 8,
            .dma_buf_len = 1024,
            .use_apll = false,
            .tx_desc_auto_clear = true,
            .fixed_mclk = 0};

        i2s_pin_config_t i2s_mic_pin_config = {
            .bck_io_num = PIN_I2S_IN_SCK,
            .ws_io_num = PIN_I2S_IN_WS,
            .data_out_num = I2S_PIN_NO_CHANGE,
            .data_in_num = PIN_I2S_IN_SD};

        bool result = false;
        result |= i2s_driver_install(I2S_NUM_1, &i2s_mic_config, 0, NULL);
        result |= i2s_set_pin(I2S_NUM_1, &i2s_mic_pin_config);

        if (result)
            log_e("Помилка ініціалізації DigitalMicrophone");
        else
            _is_inited = true;

        return !result;
    }

    void DigitalMicrophone::deinit()
    {
        if (!_is_inited)
            return;

        i2s_driver_uninstall(I2S_NUM_1);
        _is_inited = false;
    }

    size_t DigitalMicrophone::read(int16_t *out_buffer, size_t buff_len)
    {
        if (!_is_inited)
        {
            log_e("DigitalMicrophone не ініціалізовано");
            esp_restart();
        }

        size_t bytes_read;

        i2s_read(I2S_NUM_1, out_buffer, sizeof(uint16_t) * buff_len, &bytes_read, portMAX_DELAY);

        return bytes_read;
    }
}