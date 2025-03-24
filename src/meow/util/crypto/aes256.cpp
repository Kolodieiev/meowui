#pragma GCC optimize("O3")
#include "aes256.h"

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
        log_e("Encrypt err");
        return false;
    }

    esp_aes_gcm_free(&ctx);

    memcpy(out_cipher_data, iv, IV_SIZE);
    memcpy(out_cipher_data + IV_SIZE, tag, TAG_SIZE);

    return true;
}

bool aes256Decrypt(const uint8_t *aes_key, const uint8_t *cipher_data, size_t plain_data_len, uint8_t *out_plain_data)
{
    mbedtls_gcm_context ctx;
    esp_aes_gcm_init(&ctx);

    if (esp_aes_gcm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, aes_key, 256) != 0)
    {
        esp_aes_gcm_free(&ctx);
        log_e("Помилка mbedtls_gcm_setkey");
        return false;
    }

    if (esp_aes_gcm_auth_decrypt(&ctx, plain_data_len,
                                 cipher_data, IV_SIZE,
                                 NULL, 0,
                                 cipher_data + IV_SIZE, TAG_SIZE,
                                 cipher_data + IV_SIZE + TAG_SIZE,
                                 out_plain_data) != 0)
    {
        esp_aes_gcm_free(&ctx);
        log_e("Decrypt err");
        return false;
    }

    esp_aes_gcm_free(&ctx);
    return true;
}

void generateAes256Key(uint8_t *out_aes_key_buff)
{
    for (size_t i = 0; i < AES_KEY_SIZE; i += sizeof(uint32_t))
    {
        uint32_t rand_val = esp_random();
        memcpy(&out_aes_key_buff[i], &rand_val, 4);
    }
}