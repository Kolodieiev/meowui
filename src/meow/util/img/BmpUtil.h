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
        /**
         * @brief Завантажує BMP з SD-карти в ps_ram. BMP повинен мати глибину кольору 16 біт, а кодування кольору формат 565.
         *
         * @param path_to_bmp Шлях до файлу на карті пам'яті без точки монтування.
         * @return ImgData
         */
        static ImgData loadBmp(const char *path_to_bmp);

        /**
         * @brief Зберігає BMP до SD-карти. BMP повинен мати глибину кольору 16 біт, а кодування кольору формат 565.
         *
         * @param header Заголовок BMP-файлу.
         * @param buff Буфер з даними BMP.
         * @param path_to_bmp Шлях, куди повинен бути збережений файл без точки монтування.
         * @return true - Якщо BMP було успішно збережено на карту пам'яті.
         * @return false - Інакше.
         */
        static bool saveBmp(BmpHeader &header, const uint16_t *buff, const char *path_to_bmp);

        /**
         * @brief Перевіряє BMP-файл, чи відповідає той вимогам системи для використання.
         *
         * @param bmp_file Вказівник на відкритий для читання файл.
         * @param out_bmp_header Адреса об'єкта, куди будуть записані заголовкові дані bmp-файла, якщо він є валідним.
         * @return true - Якщо BMP-файл валідний, та може бути безпосередньо використаний в системі.
         * @return false - Інакше.
         */
        static bool checkBmpFile(FILE *bmp_file, BmpHeader &out_bmp_header);

    private:
        static bool validateHeader(const BmpHeader &bmp_header);
    };
}