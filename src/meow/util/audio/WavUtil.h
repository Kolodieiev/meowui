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
        const uint8_t HEADER_SIZE{44};
        FileManager _file_mngr;

        bool validateHeader(const WavHeader &wav_header);
    };
}