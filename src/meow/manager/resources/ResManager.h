#pragma once
#include <Arduino.h>
#include <unordered_map>
#include "meow/util/audio/AudioData.h"
#include "meow/util/img/ImgData.h"

namespace meow
{
    class ResManager
    {
    public:
        /**
         * @brief Завантажує bmp із карти пам'яті до PSRAM.
         *
         * @param path_to_bmp Шлях до ресурсу на карті пам'яті.
         * @return uint16_t - Унікальний ідентифікатор ресурсу, який буде більшим за 0, якщо завантаження успішне, інакаше - 0.
         */
        uint16_t loadBmpRes(const char *path_to_bmp);

        /**
         * @brief Завантажує wav із карти пам'яті до PSRAM.
         *
         * @param path_to_wav Шлях до ресурсу на карті пам'яті
         * @return uint16_t - Унікальний ідентифікатор ресурсу, який буде більшим за 0, якщо завантаження успішне, інакаше - 0.
         */
        uint16_t loadWavRes(const char *path_to_wav);

        /**
         * @brief Повертає структуру, що містить інформацію про завантажене зображення.
         *
         * @param res_id Ідентифікатор ресурсу.
         * @return ImgData - структура, що містить інформацію про зображення, або порожня структура, якщо ресурс відсутній у списку.
         */
        ImgData getBmpRes(uint16_t res_id);

        /**
         * @brief Повертає структуру, що містить інформацію про завантажену wav-доріжку.
         *
         * @param res_id
         * @return AudioData
         */
        AudioData getWavRes(uint16_t res_id);

        /**
         * @brief Видаляє зі списку ресурсів структуру, що містить інформацію про зображення, та звільняє пам'ять, яку зображення займало.
         *
         * @param res_id
         */
        void deleteBmpRes(uint16_t res_id);

        /**
         * @brief Видаляє зі списку ресурсів структуру, що містить інформацію про звукову доріжку, та звільняє пам'ять, яку вона займала.
         *
         * @param res_id
         */
        void deleteWavRes(uint16_t res_id);

        /**
         * @brief Очищує список ресурсів зображень та звільняє пам'ять, яку вони займали.
         *
         */
        void clearBmpRes();

        /**
         * @brief Очищує список ресурсів звукових доріжок та звільняє пам'ять, яку вони займали.
         *
         */
        void clearWavRes();

    private:
        uint16_t _img_res_id_i{1};
        uint16_t _audio_res_id_i{1};
        std::unordered_map<uint16_t, AudioData> _wav_res;
        std::unordered_map<uint16_t, ImgData> _bmp_res;
        AudioData _empty_audio;
        ImgData _empty_img;
    };

    /**
     * @brief Глобальний об'єкт для керування завантаженими ресурсами.
     *
     */
    extern ResManager _res;
}