#include "WavManager.h"
#include "driver/i2s.h"

namespace meow
{
    uint64_t WavManager::_track_id;
    std::unordered_map<uint16_t, WavTrack *> WavManager::_mix;

    WavManager::~WavManager()
    {
        if (_task_handle != NULL)
        {
            _params.cmd = TaskParams::CMD_STOP;
            vTaskDelete(_task_handle);
        }

        if (_is_inited)
            i2s_driver_uninstall(I2S_NUM_0);

        for (auto mix_it = _mix.begin(), last_it = _mix.end(); mix_it != last_it; ++mix_it)
            delete mix_it->second;

        _mix.clear();
        _track_id = 0;
    }

    bool WavManager::init(uint8_t I2S_BCLK_PIN, uint8_t I2S_LRC_PIN, uint8_t I2S_DOUT_PIN)
    {
        i2s_config_t i2s_config = {};

        i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
        i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
        i2s_config.sample_rate = 16000;
        i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
        i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
        i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
        i2s_config.dma_buf_count = 16;
        i2s_config.dma_buf_len = 512;
        i2s_config.use_apll = false;
        i2s_config.fixed_mclk = true;
        i2s_config.tx_desc_auto_clear = true;

        if (i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) != ESP_OK)
        {
            log_e("Помилка ініціалізації I2S драйверу");
            return false;
        }

        i2s_pin_config_t pin_config = {};

        pin_config.bck_io_num = I2S_BCLK_PIN;
        pin_config.ws_io_num = I2S_LRC_PIN;
        pin_config.data_out_num = I2S_DOUT_PIN;
        pin_config.data_in_num = I2S_PIN_NO_CHANGE;
        pin_config.mck_io_num = I2S_PIN_NO_CHANGE;

        if (i2s_set_pin(I2S_NUM_0, &pin_config) != ESP_OK)
        {
            log_e("Помилка ініціалізації I2S пінів");
            return false;
        }

        _is_inited = true;
        return true;
    }

    uint16_t WavManager::addToMix(WavTrack *sound)
    {
        if (!sound)
        {
            log_e("Помилка. sound == null");
            esp_restart();
        }

        ++_track_id;
        _mix.insert(std::pair<uint16_t, WavTrack *>(_track_id, sound));
        return _track_id;
    }

    void WavManager::removeFromMix(uint16_t id)
    {
        _mix.erase(id);
    }

    void WavManager::startMix()
    {
        if (!_is_inited)
        {
            log_e("Помилка. I2S не ініціалізовано");
            return;
        }

        if (_is_playing)
            return;

        _is_playing = true;

        xTaskCreatePinnedToCore(mixTask, "mixTask", (1024 / 2) * 10, &_params, 10, &_task_handle, 0);
    }

    void WavManager::pauseResume()
    {
        if (_params.cmd == TaskParams::CMD_NONE)
        {
            _params.cmd = TaskParams::CMD_PAUSE;
        }
        else if (_params.cmd == TaskParams::CMD_PAUSE)
        {
            _params.cmd = TaskParams::CMD_NONE;
        }
    }

    void WavManager::mixTask(void *params)
    {
        int16_t _samples_buf[256];

        TaskParams *task_params = (TaskParams *)params;

        int16_t sample;
        size_t bytes_written;

        while (task_params->cmd != TaskParams::CMD_STOP)
        {
            if (task_params->cmd != TaskParams::CMD_PAUSE)
            {
                for (int16_t i{0}; i < 256; i += 2)
                {
                    sample = 0;

                    for (auto it = _mix.begin(), last_it = _mix.end(); it != last_it; ++it)
                    {
                        if (it->second->isPlaying())
                        {
                            // TODO clipping
                            sample += it->second->getNextSample();
                        }
                        else
                        {
                            delete it->second;
                            _mix.erase(it);
                        }
                    }

                    _samples_buf[i] = sample;
                    _samples_buf[i + 1] = sample;
                }

                i2s_write(I2S_NUM_0, _samples_buf, sizeof(uint16_t) * 256, &bytes_written, 200 / portTICK_PERIOD_MS);
            }

            taskYIELD();
        }

        vTaskDelete(NULL);
    }
}