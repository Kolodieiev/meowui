#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>

namespace meow
{

    class WavTrack
    {
    public:
        WavTrack(uint8_t *data_buf, uint32_t data_size) : _data_buf{data_buf}, _data_size{data_size} {}

        void play();
        void stop() { _is_playing = false; }
        int16_t getNextSample();

        void setOnRepeat(bool repeate) { _on_repeate = repeate; }
        bool isPlaying() const { return _is_playing; };

        void setVolume(uint8_t volume);
        uint8_t getVolume() const { return _volume * MAX_VOLUME; }

        // Встановити коефіцієнт фільтрації шуму. По замовченню встановлено 1. Без фільтрації.
        void setFiltrationLvl(uint16_t lvl) { _filtration_lvl = lvl; }

        WavTrack *clone();

    private:
        WavTrack() : _data_buf{nullptr}, _data_size{0} {}

        const uint8_t MAX_VOLUME{100};
        const float DEF_VOLUME{1.0f};

        float _volume{DEF_VOLUME};
        //
        const uint8_t *_data_buf{nullptr};
        const uint32_t _data_size;
        //
        uint32_t _current_sample{0};
        //
        uint16_t _filtration_lvl{1};
        //
        bool _is_playing{true};
        bool _on_repeate{false};
        //
    };

}