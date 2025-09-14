#include "QR_Gen.h"
#pragma GCC optimize("O3")

#include "./qrencode.h"

uint16_t *QR_Gen::generateQR(const char *str, uint8_t pixel_prescaller, uint16_t front_color, uint16_t back_color, uint8_t casesensitive)
{
    QRcode *qr = QRcode_encodeString(str, 0, QR_ECLEVEL_H, QR_MODE_8, casesensitive);

    if (!qr)
    {
        log_e("Помилка кодування QRcode");
        return nullptr;
    }

    uint16_t qr_width = qr->width;

    if (pixel_prescaller == 0)
        pixel_prescaller = 1;

    _image_width = (qr_width + 6) * pixel_prescaller;

    if (_image_width % 4)
        _image_width = (_image_width / 4) * 4;

    uint32_t num_pixels = _image_width * _image_width;

    uint16_t *result_image;

    if (psramInit())
    {
        result_image = (uint16_t *)ps_malloc(num_pixels * sizeof(uint16_t));
        _in_psram = true;
    }
    else
        result_image = (uint16_t *)malloc(num_pixels * sizeof(uint16_t));

    if (!result_image)
    {
        log_e("Out of memory");
        return nullptr;
    }

    for (uint32_t i = 0; i < num_pixels; ++i)
        result_image[i] = back_color;

    unsigned char *source_data = qr->data;

    for (unsigned int y = 0; y < qr_width; ++y)
    {
        for (unsigned int x = 0; x < qr_width; ++x)
        {
            if (*source_data & 1)
            {
                for (uint16_t dy = 0; dy < pixel_prescaller; ++dy)
                    for (uint16_t dx = 0; dx < pixel_prescaller; ++dx)
                    {
                        uint16_t dest_x = (x + 3) * pixel_prescaller + dx;
                        uint16_t dest_y = (y + 3) * pixel_prescaller + dy;

                        result_image[dest_y * _image_width + dest_x] = front_color;
                    }
            }

            ++source_data;
        }
    }

    QRcode_free(qr);
    return result_image;
}