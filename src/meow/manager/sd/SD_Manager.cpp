#include "SD_Manager.h"
#include <dirent.h>
#include <sd_diskio.h>
#include <sys/stat.h>

namespace meow
{
    bool SD_Manager::isMounted() const
    {
        if (_pdrv == 0xFF)
            return false;

        String path_to_root = SD_MOUNTPOINT;
        path_to_root += "/";
        struct stat st;
        if (stat(path_to_root.c_str(), &st) != 0)
        {
            log_e("Помилка читання stat");
            return false;
        }

        return S_ISDIR(st.st_mode);
    }

    bool SD_Manager::mount(SPIClass *spi)
    {
        if (_pdrv != 0xFF)
            unmount();

        if (!spi || !spi->begin())
        {
            log_e("Некоректна шина SPI або помилка ініціалізації шини");
            return false;
        }

        _pdrv = sdcard_init(SD_PIN_CS, spi, SD_FREQUENCY);
        if (_pdrv == 0xFF)
        {
            log_e("Помилка ініціалізації SD");
            return false;
        }

        if (!sdcard_mount(_pdrv, SD_MOUNTPOINT, SD_MAX_FILES, false))
        {
            sdcard_unmount(_pdrv);
            sdcard_uninit(_pdrv);
            _pdrv = 0xFF;
            log_e("Помилка монтування SD");
            return false;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
        log_i("Карту пам'яті примонтовано");
        return true;
    }

    void SD_Manager::unmount()
    {
        sdcard_unmount(_pdrv);
        sdcard_uninit(_pdrv);
        _pdrv = 0xFF;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    SD_Manager _sd;
}
