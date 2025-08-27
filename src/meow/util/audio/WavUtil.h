#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>

#include "./WavHeader.h"
#include "./AudioData.h"

namespace meow
{
    class WavUtil
    {
    public:
        AudioData loadWav(const char *path_to_wav);

    private:
        bool validateHeader(const WavHeader &wav_header);
    };
}