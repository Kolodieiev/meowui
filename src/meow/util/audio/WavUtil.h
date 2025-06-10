#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include "../../manager/files/FileManager.h"

#include "./WavHeader.h"
#include "./AudioData.h"

namespace meow
{
    class WavUtil
    {
    public:
        AudioData loadWav(const char *path_to_wav);

    private:
        FileManager _file_mngr;
        const uint8_t HEADER_SIZE{44};

        bool validateHeader(const WavHeader &wav_header);
    };
}