#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include "./BmpHeader.h"
#include "./ImgData.h"

namespace meow
{
    class BmpUtil
    {

    public:
        // Завантажити BMP із SD карти. BMP повинен мати глибину кольору 16 біт, а кодування кольору у форматі 565
        ImgData loadBmp(const char *path_to_bmp);

        // Зберегти зображення до SD карти. Вхідні дані повинні мати глибину кольору 16 біт, а кодування кольору у форматі 565
        bool saveBmp(BmpHeader &header, const uint16_t *buff, const char *path_to_bmp);

    private:
        bool validateHeader(const BmpHeader &bmp_header);
        ImgData _empty_data;
    };
}