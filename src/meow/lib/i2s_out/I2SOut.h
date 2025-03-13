#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <driver/i2s.h>

namespace meow
{
    class I2SOut
    {
    public:
        ~I2SOut() { deinit(); }

        /**
         * @brief Ініціалізує шину I2S в режимі виводу.
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
         * @brief Відправляє дані по раніше налаштованій шині I2S.
         *
         * @param buffer Буфер даних.
         * @param buff_len Довжина буфера.
         * @param one_chann_buff Прапор, який вказує на кількість каналів, що містяться у буфері. Якщо true - буфер одноканальний. Інакше двоканальний.
         * @return size_t - Кількість записаних даних.
         */
        size_t write(const int16_t *buffer, size_t buff_len, bool one_chann_buff = true);

    private:
        static bool _is_inited;
    };
}