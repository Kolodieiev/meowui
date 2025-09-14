#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>

class QR_Gen
{
public:
    uint16_t *generateQR(const char *str, uint8_t pixel_prescaller = 3, uint16_t front_color = 0x0000, uint16_t back_color = 0xffff, uint8_t casesensitive = 1);

    // Повертає ширину зображення QR-коду.
    uint16_t getImageWidth() const { return _image_width; }

    // Повертає true, якщо вихідне зображення розміщено в PSRAM.
    bool isInPSRAM() const { return _in_psram; }

private:
    uint16_t _image_width = 0;
    bool _in_psram = false;
};
