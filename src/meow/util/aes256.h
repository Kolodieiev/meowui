#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <mbedtls/gcm.h>
#include <esp_system.h>

namespace meow
{
#define AES_KEY_SIZE 32
#define IV_SIZE 12
#define TAG_SIZE 16

    /**
     * @brief Шифрує дані в режимі GCM.
     *
     * @param aes_key 32-байтовий ключ, яким буде зашифровано дані.
     * @param plain_data Дані, які буде зашифровано.
     * @param plain_data_len Розмір даних, які повинні бути зашифровані.
     * @param out_cipher_data Вихідний буфер, куди буде записано IV + TAG + дані.
     * @return true - Якщо шифрування виконано успішно.
     * @return false - Інакше.
     */
    bool aes256Encrypt(const uint8_t *aes_key, const uint8_t *plain_data, size_t plain_data_len, uint8_t *out_cipher_data);

    /**
     * @brief Розшифровує дані в режимі GCM.
     *
     * @param aes_key 32-байтовий ключ, яким буде розшифровано дані.
     * @param cipher_data Буфер, що містить IV + TAG + зашифровані дані.
     * @param plain_data_len Розмір зашифрованих даних.
     * @param out_plain_data Вихідний буфер, куди буде записано розшифровані дані.
     * @return true - Якщо дешифрування виконано успішно.
     * @return false - Інакше.
     */
    bool aes256Decrypt(const uint8_t *aes_key, const uint8_t *cipher_data, size_t plain_data_len, uint8_t *out_plain_data);

    /**
     * @brief Генерує 32-байтовий ключ за допомоги генератора псевдовипадкових чисел.
     *
     * @param out_aes_key_buff Вихідний буфер розміром 32 байти, куди буде записано ключ.
     */
    void generateAes256Key(uint8_t *out_aes_key_buff);
}