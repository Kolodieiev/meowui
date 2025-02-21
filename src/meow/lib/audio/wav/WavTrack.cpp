#pragma GCC optimize("O3")
#include "WavTrack.h"
#include "math.h"

namespace meow
{
    void WavTrack::play()
    {
        if (_data_buf)
            _is_playing = true;

        _current_sample = 0;
    }

    int16_t WavTrack::getNextSample()
    {
        if (!_is_playing)
            return 0;

        int16_t ret = *((int16_t *)(_data_buf + _current_sample));
        ret = static_cast<int16_t>(ret * _volume);

        if (std::abs(ret) > _volume * _filtration_lvl)
        {
            int8_t high = ret;
            ret >>= 8;
            int8_t low = ret;

            ret = high;
            ret <<= 8;
            ret |= low;
        }
        else
        {
            ret = 0;
        }

        _current_sample += 2;

        if (_current_sample >= _data_size)
        {
            if (!_on_repeate)
                _is_playing = false;

            _current_sample = 0;
        }

        return ret;
    }

    void WavTrack::setVolume(uint8_t volume)
    {
        if (volume < MAX_VOLUME)
            _volume = (float)volume / 100;
        else
            _volume = DEF_VOLUME;
    }

    WavTrack *WavTrack::clone()
    {
        try
        {
            return new WavTrack(*this);
        }
        catch (const std::bad_alloc &e)
        {
            log_e("Помилка клонування звукової доріжки");
            esp_restart();
        }
    }
}