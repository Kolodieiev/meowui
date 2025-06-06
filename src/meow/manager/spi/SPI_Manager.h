#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <unordered_map>

namespace meow
{
    class SPI_Manager
    {
    public:
        bool initBus(uint8_t bus_num, int8_t sclk_pin = -1, int8_t miso_pin = -1, int8_t mosi_pin = -1);
        void deinitBus(uint8_t bus_num);
        SPIClass *getSpi4Bus(uint8_t bus_num);

    private:
        static std::unordered_map<uint8_t, SPIClass *> _spi_map;
    };
}