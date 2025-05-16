#pragma once
#include <Arduino.h>

namespace meow
{
    struct ImgData
    {
        uint16_t width{0};
        uint16_t height{0};
        const uint16_t *data_ptr{nullptr};
    };
}