#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <driver/i2s.h>

namespace meow
{
    class I2SWriter
    {
    public:
        ~I2SWriter() { deinit(); }

        /**
         * @brief Ініціалізує шину I2S в режимі виводу.
         *
         * @param port Порт, на якому буде ініціалізована шина.
         * @param buff_size Розмір буферу.
         * @param sample_rate Кількість вибірок за секунду.
         * @return true - Якщо ініціалізацію виконано успішно.
         * @return false - Інакше.
         */
        bool init(i2s_port_t port, uint16_t buff_size, uint16_t sample_rate = 16000);

        /**
         * @brief Деініціалізує ініціалізовану раніше шину I2S.
         *
         */
        void deinit();

        /**
         * @brief Відправляє дані по раніше налаштованій шині I2S.
         *
         * @param buffer Буфер даних.
         * @param one_chann_buff Прапор, який вказує на кількість каналів, що містяться у буфері. Якщо true - буфер одноканальний. Інакше двоканальний.
         * @return size_t - Кількість записаних даних.
         */
        size_t write(const void *buffer, bool one_chann_buff = true);

    private:
        bool _is_inited = false;
        i2s_port_t _port;
        uint16_t _buff_size;
    };
}