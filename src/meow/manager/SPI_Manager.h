#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <unordered_map>

namespace meow
{
    class SPI_Manager
    {
    public:
        /**
         * @brief Ініціалізує шину SPI.
         *
         * @param bus_num Номер модуля SPI. HSPI/VSPI тощо.
         * @param sclk_pin Номер піна CLK.
         * @param miso_pin Номер піна MISO.
         * @param mosi_pin Номер піна MOSI.
         * @return true - Якщо шину було успішно ініціалізовано.
         * @return false - Інакше.
         */
        static bool initBus(uint8_t bus_num, int8_t sclk_pin = -1, int8_t miso_pin = -1, int8_t mosi_pin = -1);

        /**
         * @brief Деініціалізує шину для вказаного модуля SPI, якщо її було ініціалізовано раніше.
         *
         * @param bus_num Номер модуля SPI. HSPI/VSPI тощо.
         */
        static void deinitBus(uint8_t bus_num);

        /**
         * @brief Повертає вказівник на ініціалізовану раніше шину SPI для вказаного модуля.
         *
         * @param bus_num Номер модуля SPI. HSPI/VSPI тощо.
         * @return SPIClass* - Якщо шину для вказаного модуля було ініціалізовано раніше.
         * @return nullptr - Інакше.
         */
        static SPIClass *getSpi4Bus(uint8_t bus_num);

        SPI_Manager(const SPI_Manager &) = delete;
        SPI_Manager &operator=(const SPI_Manager &) = delete;
        SPI_Manager(SPI_Manager &&) = delete;
        SPI_Manager &operator=(SPI_Manager &&) = delete;

    private:
        SPI_Manager() {}
        static std::unordered_map<uint8_t, SPIClass *> _spi_map;
    };
}