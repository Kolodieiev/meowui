#pragma once
#include <Arduino.h>

#define BMP_HEADER_SIZE 54
#define BMP_HEADER_MASKS_SIZE 12

namespace meow
{
#pragma pack(push, 1)
    struct BmpHeader
    {
        uint16_t file_type{0x4D42};
        uint32_t file_size{0};
        uint32_t reserved{0};
        uint32_t data_offset{BMP_HEADER_SIZE + BMP_HEADER_MASKS_SIZE};

        uint32_t info_size{40};
        int32_t width{0};
        int32_t height{0};
        uint16_t planes{1};
        uint16_t bit_pp{16};
        uint32_t compression{3};
        uint32_t image_size{0};
        int32_t x_pixels_per_meter{0};
        int32_t y_pixels_per_meter{0};
        uint32_t colors_used{0};
        uint32_t colors_important{0};
        //
        uint32_t red_mask{0xF800};
        uint32_t green_mask{0x07E0};
        uint32_t blue_mask{0x001F};
    };
#pragma pack(pop)

}