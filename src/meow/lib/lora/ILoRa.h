#pragma once
#include <Arduino.h>
#include "modules_setup/lora/LoRa.h"

namespace meow
{
    class ILoRa
    {
    public:
        enum TransmitPower : uint8_t
        {
            TR_POWER_MAX = 0,
            TR_POWER_HIGH,
            TR_POWER_MID,
            TR_POWER_LOW
        };

        enum Mode : uint8_t
        {
            MODE_CONFIG = 0,
            MODE_NORMAL
        };

        enum AirDataRate : uint8_t
        {
            AIR_DATA_RATE_0_3K = 0,
            AIR_DATA_RATE_1_2K = 1,
            AIR_DATA_RATE_2_4K = 2,
            AIR_DATA_RATE_4_8K = 3,
            AIR_DATA_RATE_9_6K = 4,
            AIR_DATA_RATE_19_2K = 5,
            AIR_DATA_RATE_38_4K = 6,
            AIR_DATA_RATE_62_5K = 7
        };

        ILoRa(AirDataRate min_data_rate,
              AirDataRate max_data_rate,
              uint8_t max_chann);

        virtual ~ILoRa();

        /**
         * @brief Ініціалізує шину UART для Serial1 та модуль LoRa.
         *
         * @return true - В разі успіху ініціалізації.
         * @return false - Інакше.
         */
        bool init();

        /**
         * @brief Деініціалізує шину UART для Serial1 та переводить модуль LoRa в режим сну.
         *
         */
        void end();

        /**
         * @brief Встановлює розмір пакета даних у байтах.
         * Розмір пакета не повинен перевищувати 58 байтів.
         *
         * @param len
         */
        void setPackLen(uint8_t len);

        /**
         * @brief  Повертає розмір пакета даних.
         *
         * @return uint8_t
         */
        uint8_t getPackLen() const { return _packet_len; }

        /**
         * @brief Встановлює режим для модуля.
         *
         * @param mode Може мати значення MODE_CONFIG або MODE_NORMAL.
         *
         */
        void setMode(Mode mode);

        /**
         * @brief Повертає поточний режим роботи модуля.
         *
         * @return Mode
         */
        Mode getMode() const { return _mode; }

        /**
         * @brief Встановлює номер каналу, на якому буде працювати модуль.
         * Цей метод змінює тільки локальне налаштування.
         * Для запису налаштувань в модуль, необхідно викликати writeSettings().
         *
         * @param channel Канал, на якому працюватиме модуль.
         * Канал необхідно уточнювати відповідно до модуля.
         * Якщо значення більше за максимальне для певного модуля, буде задано максимально доступне значення каналу.
         */
        void setChannel(uint8_t channel);

        /**
         * @brief Повертає локальне значення налаштування номеру каналу.
         *
         * @return uint8_t
         */
        uint8_t getChannel() const { return _channel; }

        /**
         * @brief Встановлює швидкість передачі даних модулем.
         * Цей метод змінює тільки локальне налаштування.
         * Для запису налаштувань в модуль, необхідно викликати writeSettings().
         *
         * @param rate
         */
        void setAirDataRate(AirDataRate rate);

        /**
         * @brief Повертає локальне значення налаштування швидкості передачі даних.
         *
         * @return AirDataRate
         */
        AirDataRate getAirDataRate() const { return _air_data_rate; }

        /**
         * @brief Встановлює потужність передавача.
         * Цей метод змінює тільки локальне налаштування.
         * Для запису налаштувань в модуль, необхідно викликати writeSettings().
         *
         * @param power
         */
        void setTransmitPower(TransmitPower power);

        /**
         * @brief Повертає локальне значення налаштування потужності передавача.
         *
         * @return TransmitPower
         */
        TransmitPower getTransmitPower() const { return _transmit_power; }

        /**
         * @brief Надсилає пакет, в який буде скопійовано байти із буферу, відповідно до налаштувань модуля.
         *
         * @param buff Буфер, що містить дані для пакета.
         */
        void writePacket(const uint8_t *buff);

        /**
         * @brief Читає останній пакет із модуля LoRa в буфер.
         *
         * @param out_buff Вихідний буфер, в який буде записано пакет.
         * @return true - Якщо кількість прочитаних байтів співпадає з розміром пакета, що вказаний в налаштуваннях модуля.
         * @return false - Інакше. Вихідний буфер міститиме невалідні дані, які не повинні бути оброблені.
         */
        bool readPack(uint8_t *out_buff);

        /**
         * @brief Перевіряє наявність доступного пакету у буфері даних LoRa.
         *
         * @return true - Якщо у буфері є мінімум один пакет даних.
         * @return false - Інакше.
         */
        bool packAvailable();

        /**
         * @brief Очищує усі вхідні дані із буфера LoRa.
         *
         */
        void flushBuffer();

        /**
         * @brief Повертає значення прапора, який вказує чи було модуль ініціалізовано.
         *
         * @return true - Якщо модуль було успішно ініціалізовано.
         * @return false - Інакше.
         */
        bool isInited() const { return _is_inited; }

        /**
         * @brief Записує усі налаштування модуля в його тимчасові регістри.
         *
         * @param temporary Прапор, який вказує на спосіб запису налаштувань. Тимчасово до вимкнення живлення - true(default), або зберегти в памя'ть модуля - false.
         * @return true - В разі успіху запису.
         * @return false - Інакше.
         */
        bool writeSettings(bool temporary = true);

        /**
         * @brief Повертає кількість каналів, що доступні для поточного об'єкту LoRa.
         *
         * @return uint8_t
         */
        uint8_t getMaxChann() const { return _max_chann; }

    protected:
        bool isBusy();
        void waitWhileBusy();
        bool waitResponce(size_t resp_size);

        virtual void configShadowReg() = 0;
        virtual bool writeRegisters(bool temporary) = 0;
        virtual bool readRegisters() = 0;

    private:
        const AirDataRate _min_air_data_rate{AIR_DATA_RATE_2_4K};
        const AirDataRate _max_air_data_rate{AIR_DATA_RATE_2_4K};
        AirDataRate _air_data_rate{AIR_DATA_RATE_2_4K};
        TransmitPower _transmit_power{TR_POWER_MAX};
        Mode _mode{MODE_CONFIG};

        const uint8_t _max_chann{0};
        uint8_t _channel{23};
        uint8_t _packet_len{1};

    protected:
        bool _is_inited{false};
    };
}