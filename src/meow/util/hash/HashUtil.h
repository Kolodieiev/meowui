#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>

namespace meow
{
    class HashUtil
    {
    public:
        /**
         * @brief Розраховує MD5-хеш прошивки мікроконтролера.
         *
         * @param out_buff Вказівник на буфер розміром 16 байтів, куди буде записано значення хешу.
         * @return true - Якщо хеш було розраховано успішно.
         * @return false - Інакше.
         */
        bool calcFirmwareMD5(uint8_t *out_buff) const;

        /**
         * @brief Виводить MD5-хеш в UART.
         *
         * @param out_buff Буфер, що містить MD5-хеш.
         */
        void printMD5(const uint8_t *out_buff) const;

    private:
    };
} // namespace meow
