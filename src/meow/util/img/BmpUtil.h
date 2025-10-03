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
        // Завантажити BMP з SD карти. BMP повинен мати глибину кольору 16 біт, а кодування кольору у форматі 565
        static ImgData loadBmp(const char *path_to_bmp);

        // Зберегти зображення до SD карти. Вхідні дані повинні мати глибину кольору 16 біт, а кодування кольору у форматі 565
        static bool saveBmp(BmpHeader &header, const uint16_t *buff, const char *path_to_bmp);

        /**
         * @brief Перевіряє bmp-файл, чи відповідає той вимогам системи для використання.
         *
         * @param bmp_file Вказівник на відкритий для читання файл.
         * @param out_bmp_header Адреса об'єкта, куди будуть записані заголовкові дані bmp-файла, якщо він є валідним.
         * @return true - Якщо bmp-файл валідний, та може бути безпосередньо використаний в системі.
         * @return false - Інакше.
         */
        static bool checkBmpFile(FILE *bmp_file, BmpHeader &out_bmp_header);

    private:
        static bool validateHeader(const BmpHeader &bmp_header);
    };
}