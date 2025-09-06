#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <meowui_setup/sd_setup.h>

namespace meow
{
    class SD_Manager
    {
    public:
        /**
         * @brief Монтує карту пам'яті, яка приєднана до вказаної шини SPI.
         * Якщо раніше була примонтована інша карта пам'яті, вона буде автоматично відмонтована.
         *
         * @param spi Вказівник на ініціалізовану шину SPI.
         * @return true - Якщо карту пам'яті було примонтовано.
         * @return false - Якщо під час монтування виникла помилка.
         */
        bool mount(SPIClass *spi);

        /**
         * @brief Відмонтовує примонтовану раніше карту пам'яті.
         *
         */
        void unmount();

        SD_Manager() {}
        SD_Manager(const SD_Manager &) = delete;
        SD_Manager &operator=(const SD_Manager &) = delete;
        SD_Manager(SD_Manager &&) = delete;
        SD_Manager &operator=(SD_Manager &&) = delete;

        /**
         * @brief Перевіряє чи примонтовано будь-яку карту пам'яті в даний момент.
         *
         * @return true - Якщо карту пам'яті примонтовано та вона успішно читається.
         * @return false - Якщо карта не примонтована або недоступна для читання.
         */
        bool isMounted() const;

    private:
        uint8_t _pdrv{0xFF};
    };

    /**
     * @brief Глобальний об'єкт-обгортка для керування станом карти пам'яті.
     *
     */
    extern SD_Manager _sd;
}