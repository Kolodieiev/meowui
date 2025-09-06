#pragma once
#include <Arduino.h>
#include "meowui_setup/i2c_setup.h"

namespace meow
{
    class I2C_Manager
    {
    public:
        enum I2C_MODE : uint8_t
        {
            I2C_MODE_MASTER = 0,
            I2C_MODE_SLAVE
        };

        /**
         * @brief Ініціалізує шину I2C для пристрою.
         *
         * @param mode Режим, в якому буде працювати пристрій.
         * @param slave_addr Адреса веденого пристрою для режиму I2C_MODE_SLAVE.
         * @param receive_callback Обробник вхідних даних для режиму I2C_MODE_SLAVE.
         * @param request_callback Обробник запитів даних для режиму I2C_MODE_SLAVE.
         * @return true - Якщо ініціалізацію виконано успішно.
         * @return false - Інакше.
         */
        bool begin(I2C_MODE mode = I2C_MODE_MASTER, uint8_t slave_addr = 0, void (*receive_callback)(int) = nullptr, void (*request_callback)() = nullptr);

        /**
         * @brief Деініціалізує шину I2C.
         *
         */
        void end();

        /**
         * @brief Перевіряє, чи є з'єднання з I2C-пристроєм за вказаною адресою.
         *
         * @param addr Адреса I2C-пристрою.
         * @return true - Якщо з'єднання наявне.
         * @return false - Інакше.
         */
        bool hasConnect(uint8_t addr) const;

        /**
         * @brief Записує дані до I2C-пристрою за вказаною адресою.
         *
         * @param addr Адреса I2C-пристрою.
         * @param data_buff Буфер з даними.
         * @param data_size Розмір даних.
         * @return true - Якщо дані було успішно записано.
         * @return false - Інакше.
         */
        bool write(uint8_t addr, const void *data_buff, size_t data_size) const;

        /**
         * @brief Записує дані до регістра I2C-пристрою за вказаними адресами.
         *
         * @param addr Адреса I2C-пристрою.
         * @param data_buff Буфер з даними.
         * @param reg Адреса регістра.
         * @param data_size Розмір даних.
         * @return true - Якщо дані було успішно записано.
         * @return false - Інакше.
         */
        bool writeRegister(uint8_t addr, uint8_t reg, const void *data_buff, size_t data_size) const;

        /**
         * @brief Читає дані з регістра I2C-пристрою за вказаними адресами.
         *
         * @param addr Адреса I2C-пристрою.
         * @param out_data_buff Буфер для вхідних даних.
         * @param reg Адреса регістра.
         * @param data_size Розмір даних.
         * @return true - Якщо дані було успішно прочитано.
         * @return false - Інакше.
         */
        bool readRegister(uint8_t addr, uint8_t reg, void *out_data_buff, uint8_t data_size = 1) const;

        /**
         * @brief Читає дані з I2C-пристрою за вказаною адресою.
         *
         * @param addr Адреса I2C-пристрою.
         * @param out_data_buff Буфер для вхідних даних.
         * @param data_size Розмір даних.
         * @return true - Якщо дані було успішно прочитано.
         * @return false - Інакше.
         */
        bool read(uint8_t addr, void *out_data_buff, uint8_t data_size) const;

        /**
         * @brief Розпочинає безперервне пересилання даних до I2C-пристрою за вказаною адресою.
         *
         * @param addr Адреса I2C-пристрою.
         */
        void beginTransmission(uint8_t addr) const;

        /**
         * @brief Завершує поточне безперервне пересилання даних до I2C-пристрою.
         *
         * @return true - Якщо транзакція виконана успішно.
         * @return false - Інакше.
         */
        bool endTransmission() const;

        /**
         * @brief Надсилає один байт даних до поточної відкритої транзакції з I2C-пристроєм.
         *
         * @param value Дані.
         * @return true - Якщо значення було успішно записано.
         * @return false - Інакше.
         */
        bool send(uint8_t value) const;

        /**
         * @brief Надсилає буфер даних до поточної відкритої транзакції з I2C-пристроєм.
         *
         * @param data_buff Буфер з даними.
         * @param data_size Розмір даних.
         * @return true - Якщо дані було успішно записано.
         * @return false - Інакше.
         */
        bool send(const void *data_buff, size_t data_size) const;

        /**
         * @brief Читає дані з відкритої транзакції з I2C-пристроєм.
         *
         * @param out_data_buff Буфер, до якого будуть записані прочитані дані.
         * @return true - Якщо дані було успішно прочитано.
         * @return false - Інакше.
         */
        bool receive(void *out_data_buff) const;

        /**
         * @brief Повертає стан ініціалізації шини I2C.
         *
         * @return true - Якщо шина була ініціалізована.
         * @return false - Інакше.
         */
        bool isInited() const;

        I2C_Manager() {}

        I2C_Manager(const I2C_Manager &) = delete;
        I2C_Manager &operator=(const I2C_Manager &) = delete;

        I2C_Manager(I2C_Manager &&) = delete;
        I2C_Manager &operator=(I2C_Manager &&) = delete;

    private:
        bool _is_inited = false;
    };

    /**
     * @brief Глобальний об'єкт-обгортка для роботи з шиною I2C.
     *
     */
    extern I2C_Manager _i2c;
}