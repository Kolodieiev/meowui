#pragma GCC optimize("O3")
#include "hash_util.h"

#include "spi_flash_mmap.h"
#include "esp_system.h"
#include "esp_ota_ops.h"
#include "mbedtls/md5.h"

namespace meow
{
    bool calcFirmwareMD5(uint8_t *out_buff)
    {
        const esp_partition_t *running_partition = esp_ota_get_running_partition();
        spi_flash_mmap_handle_t handle;
        const void *mapped_region;

        esp_err_t err = spi_flash_mmap(running_partition->address, running_partition->size, SPI_FLASH_MMAP_DATA, &mapped_region, &handle);
        if (err != ESP_OK)
        {
            log_e("Error mapping flash: %s\n", esp_err_to_name(err));
            return false;
        }

        mbedtls_md5_context ctx;
        mbedtls_md5_init(&ctx);
        mbedtls_md5_starts(&ctx);
        mbedtls_md5_update(&ctx, static_cast<const unsigned char *>(mapped_region), running_partition->size);
        mbedtls_md5_finish(&ctx, out_buff);
        mbedtls_md5_free(&ctx);

        spi_flash_munmap(handle);

        return true;
    }

    void printMD5(const uint8_t *out_buff)
    {
        for (size_t i = 0; i < 16; i++)
            log_i("%02x", out_buff[i]);
    }
}