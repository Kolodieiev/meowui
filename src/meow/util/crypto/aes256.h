#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <mbedtls/gcm.h>
#include <esp_system.h>

#define AES_KEY_SIZE 32
#define IV_SIZE 12
#define TAG_SIZE 16

bool aes256Encrypt(const uint8_t *aes_key, const uint8_t *plain_data, size_t plain_data_len, uint8_t *out_cipher_data)
{
    mbedtls_gcm_context ctx;
    esp_aes_gcm_init(&ctx);

    if (esp_aes_gcm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, aes_key, 256) != 0)
    {
        esp_aes_gcm_free(&ctx);
        log_e("Помилка mbedtls_gcm_setkey");
        return false;
    }

    uint8_t iv[IV_SIZE];
    uint8_t tag[TAG_SIZE];

    esp_fill_random(iv, IV_SIZE);

    if (esp_aes_gcm_crypt_and_tag(&ctx, MBEDTLS_GCM_ENCRYPT,
                                  plain_data_len,
                                  iv, IV_SIZE,
                                  NULL, 0,
                                  plain_data,
                                  out_cipher_data + IV_SIZE + TAG_SIZE,
                                  TAG_SIZE, tag) != 0)
    {
        esp_aes_gcm_free(&ctx);
        log_e("Помилка шифрування даних");
        return false;
    }

    esp_aes_gcm_free(&ctx);

    memcpy(out_cipher_data, iv, IV_SIZE);
    memcpy(out_cipher_data + IV_SIZE, tag, TAG_SIZE);

    return true;
}

bool aes256Decrypt(const uint8_t *aes_key, const uint8_t *cipher_data, size_t data_len, uint8_t *out_plain_data)
{
    if (data_len < IV_SIZE + TAG_SIZE + 1)
    {
        log_e("Некоректний розмір даних");
        return false;
    }

    mbedtls_gcm_context ctx;
    esp_aes_gcm_init(&ctx);

    if (esp_aes_gcm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, aes_key, 256) != 0)
    {
        esp_aes_gcm_free(&ctx);
        log_e("Помилка mbedtls_gcm_setkey");
        return false;
    }

    if (esp_aes_gcm_auth_decrypt(&ctx, data_len - IV_SIZE - TAG_SIZE,
                                 cipher_data, IV_SIZE,
                                 NULL, 0,
                                 cipher_data + IV_SIZE, TAG_SIZE,
                                 cipher_data + IV_SIZE + TAG_SIZE,
                                 out_plain_data) != 0)
    {
        esp_aes_gcm_free(&ctx);
        log_e("Помилка декодування даних");
        return false;
    }

    esp_aes_gcm_free(&ctx);
    return true;
}

// void test()
// {
//     String test_text = "hello from aes256!";
//     uint8_t encrypted_data[test_text.length() + IV_SIZE + TAG_SIZE];

//     unsigned long start_time = micros();
//     if (aes256Encrypt((const uint8_t *)test_text.c_str(), test_text.length(), encrypted_data))
//     {
//         log_i("Time aes256Encrypt: %lu", micros() - start_time);

//         uint8_t decrypted_data[test_text.length()];
//         start_time = micros();

//         if (aes256Decrypt(encrypted_data, test_text.length() + IV_SIZE + TAG_SIZE, decrypted_data))
//         {
//             log_i("Time aes256Decrypt: %lu", micros() - start_time);

//             for (uint8_t i = 0; i < test_text.length(); ++i)
//                 log_i("%c", decrypted_data[i]);
//         }
//     }
// }