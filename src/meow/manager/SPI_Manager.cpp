#include "SPI_Manager.h"

namespace meow
{
    std::unordered_map<uint8_t, SPIClass *> SPI_Manager::_spi_map;

    bool SPI_Manager::initBus(uint8_t bus_num, int8_t sclk_pin, int8_t miso_pin, int8_t mosi_pin)
    {
        auto it = _spi_map.find(bus_num);
        if (it != _spi_map.end())
            return true;

        try
        {
            SPIClass *spi = new SPIClass(bus_num);
            spi->begin(sclk_pin, miso_pin, mosi_pin);
            spiSSDisable(spi->bus());
            spiSSClear(spi->bus());

            _spi_map.insert({bus_num, spi});
            return true;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            return false;
        }
    }

    void SPI_Manager::deinitBus(uint8_t bus_num)
    {
        auto it = _spi_map.find(bus_num);
        if (it == _spi_map.end())
            return;

        it->second->end();
        delete (it->second);
        _spi_map.erase(it);
    }

    SPIClass *SPI_Manager::getSpi4Bus(uint8_t bus_num)
    {
        auto it = _spi_map.find(bus_num);

        if (it == _spi_map.end())
            return nullptr;

        return it->second;
    }
}
