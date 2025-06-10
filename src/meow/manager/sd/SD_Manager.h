#pragma once
#include <Arduino.h>
#include <SPI.h>

namespace meow
{
    class SD_Manager
    {
    public:
        void setup(uint8_t cs_pin = SS, SPIClass *spi = &SPI, uint32_t frequency = 4000000U, const char *mountpoint = "/sd", uint8_t max_files = (uint8_t)5U);
        bool mount();
        void unmount();
        static SD_Manager &getInst();

        SD_Manager(const SD_Manager &) = delete;
        SD_Manager &operator=(const SD_Manager &) = delete;

        SD_Manager(SD_Manager &&) = delete;
        SD_Manager &operator=(SD_Manager &&) = delete;

    private:
        SD_Manager() {}

    private:
        static bool _is_inited;

        // ----------------------------------------
        SPIClass *_spi{nullptr};
        const char *_mountpoint{nullptr};

        uint32_t _frequency{0};

        uint8_t _pdrv{0xFF};
        uint8_t _cs_pin{0};
        uint8_t _max_files{20};
        
        bool _has_setup{false};
    };
}