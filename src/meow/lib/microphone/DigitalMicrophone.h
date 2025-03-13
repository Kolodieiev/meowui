#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <driver/i2s.h>

namespace meow
{
    class DigitalMicrophone
    {
    public:
        ~DigitalMicrophone() { deinit(); }

        /**
         * @brief Ініціалізує шину I2S в режимі вводу.
         *
         * @param sample_rate Кількість вибірок за секунду.
         * @return true - Якщо ініціалізацію виконано успішно.
         * @return false - Інакше.
         */
        bool init(uint16_t sample_rate = 16000);

        /**
         * @brief Деініціалізує ініціалізовану раніше шину I2S.
         *
         */
        void deinit();

        /**
         * @brief Читає дані по раніше налаштованій шині I2S.
         *
         * @param out_buffer Буфер даних.
         * @param buff_len Довжина буфера.
         * @return size_t - Кількість прочитаних байтів.
         */
        size_t read(int16_t *out_buffer, size_t buff_len);

    private:
        static bool _is_inited;
    };
}