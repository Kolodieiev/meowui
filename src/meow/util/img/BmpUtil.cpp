#pragma GCC optimize("O3")
#include "BmpUtil.h"
#include "meow/manager/files/FileManager.h"

namespace meow
{
    ImgData BmpUtil::loadBmp(const char *path_to_bmp)
    {
        if (!_fs.fileExist(path_to_bmp))
            return _empty_data;

        FILE *f = _fs.openFile(path_to_bmp, "r");

        if (!f)
            return _empty_data;

        BmpHeader bmp_header;

        _fs.readFromFile(f, &bmp_header, BMP_HEADER_SIZE);

        if (!validateHeader(bmp_header))
        {
            _fs.closeFile(f);
            log_e("Помилка валідації файлу: %s", path_to_bmp);
            return _empty_data;
        }

        if (!psramInit())
        {
            _fs.closeFile(f);
            log_e("Помилка ініціалізації PSRAM");
            return _empty_data;
        }
        //
        bool is_flipped = bmp_header.height > 0;

        uint16_t width = static_cast<uint16_t>(bmp_header.width);
        uint16_t height = static_cast<uint16_t>(std::abs(bmp_header.height));
        //
        size_t data_size = static_cast<size_t>(width * height * 2);
        //
        uint8_t *data = static_cast<uint8_t *>(ps_malloc(data_size));
        if (!data)
        {
            _fs.closeFile(f);
            log_e("Помилка виділення %zu байтів PSRAM", data_size);
            return _empty_data;
        }

        if (!_fs.readFromFile(f, data, data_size, bmp_header.data_offset))
        {
            log_e("Помилка читання файлу: %s", path_to_bmp);
            free(data);
            _fs.closeFile(f);
            return _empty_data;
        }

        _fs.closeFile(f);

        uint16_t *data_temp = reinterpret_cast<uint16_t *>(data);

        if (is_flipped)
        {
            uint32_t d_size = width * height;
            for (uint32_t i = 0; i < d_size / 2; ++i)
            {
                uint16_t temp = data_temp[i];
                data_temp[i] = data_temp[d_size - i - 1];
                data_temp[d_size - i - 1] = temp;
            }
        }

        for (uint32_t y = 0; y < height; ++y)
        {
            uint16_t *row_start = data_temp + y * width;

            for (uint32_t x = 0; x < width / 2; ++x)
            {
                uint16_t temp = row_start[x];
                row_start[x] = row_start[width - x - 1];
                row_start[width - x - 1] = temp;
            }
        }

        ImgData bmp_data;
        bmp_data.width = width;
        bmp_data.height = height;
        bmp_data.data_ptr = reinterpret_cast<uint16_t *>(data);

        return bmp_data;
    }

    bool BmpUtil::validateHeader(const BmpHeader &bmp_header)
    {
        if ((bmp_header.file_type != 0x4D42))
        {
            log_e("Не bmp");
            return false;
        }

        if ((bmp_header.bit_pp != 16))
        {
            log_e("Зображення повинне мати 16bpp");
            return false;
        }

        if ((bmp_header.width == 0 || bmp_header.height == 0))
        {
            log_e("Зображення містить некоректний заголовок");
            return false;
        }

        return true;
    }

    bool BmpUtil::saveBmp(BmpHeader &header, const uint16_t *buff, const char *path_to_bmp)
    {
        header.image_size = header.width * header.height * 2;
        header.file_size = header.data_offset + header.image_size;
        uint32_t buf_size = header.width * header.height;
        header.height *= -1;

        uint8_t *data = static_cast<uint8_t *>(ps_malloc(header.file_size));
        if (!data)
        {
            log_e("Помилка виділення %lu байт PSRAM", header.file_size);
            return false;
        }

        const uint8_t *header_ptr = reinterpret_cast<uint8_t *>(&header);
        for (uint32_t i = 0; i < header.data_offset; ++i)
            data[i] = header_ptr[i];

        uint16_t *data_p16 = reinterpret_cast<uint16_t *>(data + header.data_offset);
        for (int i = 0; i < buf_size; ++i)
            data_p16[i] = __bswap16(buff[i]);
        //
        size_t written_bytes = _fs.writeFile(path_to_bmp, data, header.file_size);

        free(data);

        return written_bytes == header.file_size;
    }
}