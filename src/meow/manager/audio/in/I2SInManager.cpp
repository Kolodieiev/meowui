#include "I2SInManager.h"
#include "meowui_setup/i2s_setup.h"

#define MIN_MICRO_BUFF_LEN 40UL

namespace meow
{
    bool I2SInManager::_is_inited{false};

    bool I2SInManager::init(uint32_t sample_rate, bool has_interleaving)
    {
        if (_is_inited)
            return true;

        _has_interleaving = has_interleaving;

        bool result = false;

        _i2s_chan_cfg.id = I2S_NUM_1;
        _i2s_chan_cfg.role = I2S_ROLE_MASTER;
        _i2s_chan_cfg.dma_desc_num = 8;
        _i2s_chan_cfg.dma_frame_num = 1024;
        _i2s_chan_cfg.auto_clear = true;
        result |= i2s_new_channel(&_i2s_chan_cfg, nullptr, &_i2s_rx_handle);

        if (result == ESP_OK)
        {
            _i2s_rx_std_cfg.slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
            _i2s_rx_std_cfg.gpio_cfg.mclk = I2S_GPIO_UNUSED;
            _i2s_rx_std_cfg.gpio_cfg.bclk = (gpio_num_t)PIN_I2S_IN_SCK;
            _i2s_rx_std_cfg.gpio_cfg.ws = (gpio_num_t)PIN_I2S_IN_WS;
            _i2s_rx_std_cfg.gpio_cfg.din = (gpio_num_t)PIN_I2S_IN_SD;
            _i2s_rx_std_cfg.gpio_cfg.dout = I2S_GPIO_UNUSED;
            _i2s_rx_std_cfg.gpio_cfg.invert_flags.mclk_inv = false;
            _i2s_rx_std_cfg.gpio_cfg.invert_flags.bclk_inv = false;
            _i2s_rx_std_cfg.gpio_cfg.invert_flags.ws_inv = false;

            _i2s_rx_std_cfg.clk_cfg.clk_src = I2S_CLK_SRC_DEFAULT;
            _i2s_rx_std_cfg.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_128;
            _i2s_rx_std_cfg.clk_cfg.sample_rate_hz = sample_rate;

            result |= i2s_channel_init_std_mode(_i2s_rx_handle, &_i2s_rx_std_cfg);
        }

        if (result != ESP_OK)
            log_e("Помилка ініціалізації I2SInManager");
        else
        {
            _is_inited = true;
            clearBuffer();
        }

        return !result;
    }

    bool I2SInManager::isInited() const
    {
        if (!_is_inited)
            log_e("I2S вхід не ініціалізовано");

        return _is_inited;
    }

    void I2SInManager::deinit()
    {
        if (!_is_inited)
            return;

        i2s_channel_disable(_i2s_rx_handle);
        i2s_del_channel(_i2s_rx_handle);
        _is_inited = false;
    }

    void I2SInManager::clearBuffer()
    {
        if (!_is_inited)
            return;

        uint8_t *buff = static_cast<uint8_t *>(calloc(128, sizeof(uint8_t)));
        size_t bytes_loaded = 0;
        i2s_channel_preload_data(_i2s_rx_handle, buff, sizeof(buff), &bytes_loaded);
        free(buff);
    }

    esp_err_t I2SInManager::enable()
    {
        if (!_is_inited)
            return ESP_FAIL;

        return i2s_channel_enable(_i2s_rx_handle);
    }

    void I2SInManager::disable()
    {
        if (_is_inited)
            i2s_channel_disable(_i2s_rx_handle);
    }

    void I2SInManager::reconfigSampleRate(uint32_t sample_rate)
    {
        if (_is_inited)
        {
            i2s_channel_disable(_i2s_rx_handle);
            _i2s_rx_std_cfg.clk_cfg.sample_rate_hz = sample_rate;
            i2s_channel_reconfig_std_clock(_i2s_rx_handle, &_i2s_rx_std_cfg.clk_cfg);
            clearBuffer();
            i2s_channel_enable(_i2s_rx_handle);
        }
    }

    size_t I2SInManager::read(int16_t *out_buffer, size_t buff_len)
    {
        if (!_is_inited)
        {
            log_e("I2SInManager не ініціалізовано");
            esp_restart();
        }

        if (buff_len < MIN_MICRO_BUFF_LEN)
        {
            log_e("Мінімальний розмір буфера мікрофону повинен бути не менше %zu", MIN_MICRO_BUFF_LEN);
            esp_restart();
        }

        if (buff_len & 1)
            --buff_len;

        if (_has_interleaving)
            buff_len *= 2;

        size_t bytes_to_read = buff_len * sizeof(int32_t);
        int32_t raw_buffer[buff_len];
        size_t bytes_read = 0;

        i2s_channel_read(_i2s_rx_handle, raw_buffer, bytes_to_read, &bytes_read, portMAX_DELAY);

        size_t samples_read = bytes_read / sizeof(int32_t);

        if (_has_interleaving)
        {
            samples_read /= 2;

            for (size_t i = 0; i < samples_read; ++i)
                out_buffer[i] = raw_buffer[i * 2] >> 12;
        }
        else
        {
            for (size_t i = 0; i < samples_read; ++i)
                out_buffer[i] = raw_buffer[i] >> 12;
        }

        return samples_read;
    }

    I2SInManager _i2s_in;
}