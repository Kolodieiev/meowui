#include "WavManager.h"
#include "meow/manager/audio/out/I2SOutManager.h"

namespace meow
{
    uint64_t WavManager::_track_id;
    std::unordered_map<uint16_t, WavTrack *> WavManager::_mix;

    WavManager::~WavManager()
    {
        if (_task_handle)
        {
            _params.cmd = TaskParams::CMD_STOP;
            vTaskDelete(_task_handle);
        }

        for (auto mix_it = _mix.begin(), last_it = _mix.end(); mix_it != last_it; ++mix_it)
            delete mix_it->second;

        _mix.clear();
        _track_id = 0;
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
        if (!_i2s_out.isInited())
            esp_restart();

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
        uint32_t cycles_counter = 0;

        while (task_params->cmd != TaskParams::CMD_STOP)
        {
            if (task_params->cmd != TaskParams::CMD_PAUSE)
            {
                for (uint32_t i{0}; i < 256u; i += 2u)
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

                _i2s_out.write(_samples_buf, sizeof(uint16_t) * 256);
            }

            ++cycles_counter;

            if (cycles_counter > 50u)
            {
                cycles_counter = 0;
                vTaskDelay(1 / portTICK_PERIOD_MS);
            }
        }

        vTaskDelete(NULL);
    }
}