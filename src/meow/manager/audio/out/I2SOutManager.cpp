#include "I2SOutManager.h"
#include "meowui_setup/i2s_setup.h"

namespace meow
{
    bool I2SOutManager::_is_inited{false};

    bool I2SOutManager::init(uint32_t sample_rate)
    {
        if (_is_inited)
            return true;

        bool result = false;

        _i2s_chan_cfg.id = I2S_NUM_0;
        _i2s_chan_cfg.role = I2S_ROLE_MASTER;
        _i2s_chan_cfg.dma_desc_num = 8;
        _i2s_chan_cfg.dma_frame_num = 1024;
        _i2s_chan_cfg.auto_clear = true;
        result |= i2s_new_channel(&_i2s_chan_cfg, &_i2s_tx_handle, NULL);

        if (result == ESP_OK)
        {
            if (!_comm_fmt)
                _i2s_tx_std_cfg.slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
            else
                _i2s_tx_std_cfg.slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);

            _i2s_tx_std_cfg.gpio_cfg.bclk = (gpio_num_t)PIN_I2S_OUT_BCLK;
            _i2s_tx_std_cfg.gpio_cfg.din = I2S_GPIO_UNUSED;
            _i2s_tx_std_cfg.gpio_cfg.dout = (gpio_num_t)PIN_I2S_OUT_DOUT;
            _i2s_tx_std_cfg.gpio_cfg.mclk = I2S_GPIO_UNUSED;
            _i2s_tx_std_cfg.gpio_cfg.ws = (gpio_num_t)PIN_I2S_OUT_LRC;
            _i2s_tx_std_cfg.gpio_cfg.invert_flags.mclk_inv = false;
            _i2s_tx_std_cfg.gpio_cfg.invert_flags.bclk_inv = false;
            _i2s_tx_std_cfg.gpio_cfg.invert_flags.ws_inv = false;
            _i2s_tx_std_cfg.clk_cfg.clk_src = I2S_CLK_SRC_DEFAULT;
            _i2s_tx_std_cfg.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_128;
            _i2s_tx_std_cfg.clk_cfg.sample_rate_hz = sample_rate;
            result |= i2s_channel_init_std_mode(_i2s_tx_handle, &_i2s_tx_std_cfg);
        }

        if (result != ESP_OK)
        {
            log_e("Помилка ініціалізації I2SOutManager");
        }
        else
        {
            _is_inited = true;
            clearBuffer();
        }

        return !result;
    }

    void I2SOutManager::reconfigSampleRate(uint32_t sample_rate)
    {
        if (_is_inited)
        {
            i2s_channel_disable(_i2s_tx_handle);
            _i2s_tx_std_cfg.clk_cfg.sample_rate_hz = sample_rate;
            i2s_channel_reconfig_std_clock(_i2s_tx_handle, &_i2s_tx_std_cfg.clk_cfg);
            clearBuffer();
            i2s_channel_enable(_i2s_tx_handle);
        }
    }

    void I2SOutManager::setI2SCommFMT_LSB(bool comm_fmt)
    {
        _comm_fmt = comm_fmt;

        if (_is_inited)
        {
            i2s_channel_disable(_i2s_tx_handle);
            if (!_comm_fmt)
                _i2s_tx_std_cfg.slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
            else
                _i2s_tx_std_cfg.slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
            i2s_channel_reconfig_std_slot(_i2s_tx_handle, &_i2s_tx_std_cfg.slot_cfg);
            i2s_channel_enable(_i2s_tx_handle);
        }
    }

    size_t I2SOutManager::write(const int16_t *buffer, size_t buff_len, bool only_left_chan)
    {
        if (!_is_inited)
        {
            log_e("I2SOutManager не ініціалізовано");
            esp_restart();
        }

        if (buff_len == 0u)
            return 0u;

        size_t bytes_written{0};

        if (only_left_chan)
        {
            int16_t buffer_copy[buff_len];
            int16_t *dst = buffer_copy;

            size_t num_real_mono_samples = buff_len / 2;
            for (size_t i = 0; i < num_real_mono_samples; ++i)
            {
                int16_t mono_sample = *buffer;
                *dst++ = mono_sample;
                *dst++ = mono_sample;
                buffer += 2;
            }

            i2s_channel_write(_i2s_tx_handle, buffer_copy, buff_len * sizeof(int16_t), &bytes_written, portMAX_DELAY);
        }
        else
        {
            i2s_channel_write(_i2s_tx_handle, buffer, buff_len * sizeof(int16_t), &bytes_written, portMAX_DELAY);
        }

        return bytes_written;
    }

    bool I2SOutManager::isInited() const
    {
        if (!_is_inited)
            log_e("I2S вихід не ініціалізовано");

        return _is_inited;
    }

    void I2SOutManager::deinit()
    {
        if (!_is_inited)
            return;

        i2s_channel_disable(_i2s_tx_handle);
        i2s_del_channel(_i2s_tx_handle);

        pinMode(PIN_I2S_OUT_BCLK, OUTPUT);
        digitalWrite(PIN_I2S_OUT_BCLK, HIGH);

        pinMode(PIN_I2S_OUT_LRC, OUTPUT);
        digitalWrite(PIN_I2S_OUT_LRC, HIGH);

        pinMode(PIN_I2S_OUT_DOUT, OUTPUT);
        digitalWrite(PIN_I2S_OUT_DOUT, HIGH);

        _is_inited = false;
    }

    esp_err_t I2SOutManager::enable()
    {
        if (!_is_inited)
            return ESP_FAIL;

        return i2s_channel_enable(_i2s_tx_handle);
    }

    void I2SOutManager::disable()
    {
        if (_is_inited)
            i2s_channel_disable(_i2s_tx_handle);
    }

    void I2SOutManager::clearBuffer()
    {
        if (!_is_inited)
            return;

        uint8_t *buff = static_cast<uint8_t *>(calloc(128, sizeof(uint8_t)));
        size_t bytes_loaded = 0;
        i2s_channel_preload_data(_i2s_tx_handle, buff, sizeof(buff), &bytes_loaded);
        free(buff);
    }

    I2SOutManager _i2s_out;
}