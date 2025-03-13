#include "SD_Manager.h"
#include <dirent.h>
#include <sd_diskio.h>
#include <sys/stat.h>

namespace meow
{
    void SD_Manager::setup(uint8_t cs_pin, SPIClass *spi, uint32_t frequency, const char *mountpoint, uint8_t max_files)
    {
        _cs_pin = cs_pin;
        _spi = spi;
        _frequency = frequency;
        _mountpoint = mountpoint;
        _max_files = max_files;
        _has_setup = true;
    }

    bool SD_Manager::mount()
    {
        if (_pdrv != 0xFF)
            return true;

        if (!_has_setup)
            setup();

        _spi->begin();

        _pdrv = sdcard_init(_cs_pin, _spi, _frequency);
        if (_pdrv == 0xFF)
        {
            log_e("Помилка ініціалізації SD");
            return false;
        }

        if (!sdcard_mount(_pdrv, _mountpoint, _max_files, false) || _pdrv == 0xFF)
        {
            sdcard_unmount(_pdrv);
            sdcard_uninit(_pdrv);
            _pdrv = 0xFF;
            log_e("Помилка монтування SD");
            return false;
        }

        String path_to_root = _mountpoint;
        path_to_root += "/";

        struct stat st;
        if (stat(path_to_root.c_str(), &st) != 0)
        {
            log_e("Помилка читання stat");
            return false;
        }

        return S_ISDIR(st.st_mode);
    }

    void SD_Manager::unmount()
    {
        sdcard_unmount(_pdrv);
        _pdrv = 0xFF;
    }

    SD_Manager& SD_Manager::getInst()
    {
        static SD_Manager instance;
        return instance;
    }
}
