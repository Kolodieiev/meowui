#pragma once
#include <Arduino.h>

namespace meow
{
    class I2C_Manager
    {
    public:
        I2C_Manager();

        /**
         * @brief Ініціалізує шину I2C.
         *
         * @return true - Якщо ініціалізацію виконано успішно.
         * @return false - Інакше.
         */
        bool begin();

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
         * @param data_buff Буфер вхідних даних.
         * @param addr Адреса I2C-пристрою.
         * @param data_size Розмір даних.
         * @return true - Якщо дані було успішно записано.
         * @return false - Інакше.
         */
        bool write(const void *data_buff, uint8_t addr, size_t data_size) const;

        /**
         * @brief Записує дані до регістра I2C-пристрою за вказаними адресами.
         *
         * @param data_buff Буфер вхідних даних.
         * @param addr Адреса I2C-пристрою.
         * @param reg Адреса регістра.
         * @param data_size Розмір даних.
         * @return true - Якщо дані було успішно записано.
         * @return false - Інакше.
         */
        bool writeRegister(const void *data_buff, uint8_t addr, uint8_t reg, size_t data_size) const;

        /**
         * @brief Читає дані з регістра I2C-пристрою за вказаними адресами.
         *
         * @param out_byte_buff Буфер для вихідних даних.
         * @param addr Адреса I2C-пристрою.
         * @param reg Адреса регістра.
         * @param data_size Розмір даних.
         * @return true - Якщо дані було успішно прочитано.
         * @return false - Інакше.
         */
        bool readRegister(uint8_t *out_byte_buff, uint8_t addr, uint8_t reg, uint8_t data_size = 1) const;

        /**
         * @brief Читає дані з I2C-пристрою за вказаною адресою.
         *
         * @param out_data_buff Буфер для вихідних даних.
         * @param addr Адреса I2C-пристрою.
         * @param data_size Розмір даних.
         * @return true - Якщо дані було успішно прочитано.
         * @return false - Інакше.
         */
        bool read(uint8_t *out_data_buff, uint8_t addr, uint8_t data_size) const;

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
         * @param data_buff Буфер вхідних даних.
         * @param data_size Розмір даних.
         * @return true - Якщо дані було успішно записано.
         * @return false - Інакше.
         */
        bool send(const void *data_buff, size_t data_size) const;

    private:
        static bool _is_inited;

        bool checkInit() const;
    };
}