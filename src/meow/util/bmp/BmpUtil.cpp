#pragma GCC optimize("O3")
#include "BmpUtil.h"
#include "../../manager/files/FileManager.h"

namespace meow
{
    BmpData BmpUtil::loadBmp(const char *path_to_bmp)
    {
        FileManager f_mgr;

        if (!f_mgr.fileExist(path_to_bmp))
            return srcNotFound();

        FILE *f = f_mgr.getFileDescriptor(path_to_bmp, "r");

        if (!f)
            return srcNotFound();

        BmpHeader bmp_header;

        f_mgr.readFromFile(f, &bmp_header, BMP_HEADER_SIZE);

        if (!validateHeader(bmp_header))
        {
            f_mgr.closeFile(f);
            log_e("Помилка валідації файлу: %s", path_to_bmp);
            return srcNotFound();
        }

        if (!psramInit())
        {
            f_mgr.closeFile(f);
            log_e("Помилка ініціалізації PSRAM");
            return srcNotFound();
        }
        //
        bool is_flipped = bmp_header.height > 0;

        uint16_t width = static_cast<uint16_t>(bmp_header.width);
        uint16_t height = static_cast<uint16_t>(std::abs(bmp_header.height));

        //
        size_t data_size = static_cast<size_t>(width * height * 2);
        //
        uint8_t *data = (uint8_t *)ps_malloc(data_size);
        if (!data)
        {
            f_mgr.closeFile(f);
            log_e("Помилка виділення %zu байтів PSRAM", data_size);
            return srcNotFound();
        }

        if (!f_mgr.readFromFile(f, data, data_size, bmp_header.data_offset))
        {
            log_e("Помилка читання файлу: %s", path_to_bmp);
            free(data);
            f_mgr.closeFile(f);
            return srcNotFound();
        }

        f_mgr.closeFile(f);

        uint16_t *data_temp = (uint16_t *)data;

        if (is_flipped)
        {
            uint16_t temp;
            uint32_t d_size = width * height;

            for (uint32_t i = 0; i < d_size / 2; ++i)
            {
                temp = data_temp[i];
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

        BmpData bmp_data;
        bmp_data.width = width;
        bmp_data.height = height;
        bmp_data.is_loaded = true;
        bmp_data.data_ptr = (uint16_t *)data;

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

    BmpData BmpUtil::srcNotFound()
    {
        BmpData bmp_data;
        bmp_data.width = 32;
        bmp_data.height = 32;
        bmp_data.data_ptr = SRC_NOT_FOUND;
        return bmp_data;
    }

    bool BmpUtil::saveBmp(BmpHeader &header, const uint16_t *buff, const char *path_to_bmp)
    {
        header.image_size = header.width * header.height * 2;
        header.file_size = header.data_offset + header.image_size;
        uint32_t buf_size = header.width * header.height;
        header.height *= -1;

        uint8_t *data = (uint8_t *)ps_malloc(header.file_size);
        if (!data)
        {
            log_e("Помилка виділення %lu байт PSRAM", header.file_size);
            return false;
        }

        uint8_t *header_ptr = (uint8_t *)&header;
        for (uint8_t i = 0; i < header.data_offset; ++i)
            data[i] = header_ptr[i];

        uint16_t *data_p16 = reinterpret_cast<uint16_t *>(data + header.data_offset);
        for (int i = 0; i < buf_size; ++i)
            data_p16[i] = __bswap16(buff[i]);
        //
        FileManager f_mgr;
        size_t written_bytes = f_mgr.writeFile(path_to_bmp, data, header.file_size);

        free(data);

        return written_bytes == header.file_size;
    }
}