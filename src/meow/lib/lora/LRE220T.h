#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>

namespace meow
{
    class LRE220T
    {
    public:
        // ----------------------------------------------------------------

        enum TransmitPower : uint8_t
        {
            POWER_MAX = 0,
            POWER_HIGH,
            POWER_MID,
            POWER_LOW
        };

        enum LRE220T_Mode : uint8_t
        {
            MODE_CONFIG,
            MODE_NORMAL
        };

        enum AirDataRate : uint8_t
        {
            AIR_DATA_RATE_2_4K = 2,
            AIR_DATA_RATE_4_8K = 3,
            AIR_DATA_RATE_9_6K = 4,
            AIR_DATA_RATE_19_2K = 5,
            AIR_DATA_RATE_38_4K = 6,
            AIR_DATA_RATE_62_5K = 7
        };

        enum PacketLen : uint8_t
        {
            PACK_LEN_200B = 0,
            PACK_LEN_128B = 1,
            PACK_LEN_64B = 2,
            PACK_LEN_32B = 3
        };

        // ----------------------------------------------------------------

        /**
         * @brief Ініціалізує шину UART для Serial1 та модуль LoRa, який повинен бути підключеним до цієї шини.
         *
         * @return true - В разі успіху ініціалізації.
         * @return false - Інакше.
         */
        bool begin();

        /**
         * @brief Деініціалізує шину UART для Serial1 та переводить модуль LoRa в режим сну.
         *
         */
        void end();

        /**
         * @brief Записує усі налаштування модуля в його тимчасові регістри.
         *
         * @return true - В разі успіху запису.
         * @return false - Інакше.
         */
        bool writeSettings();

        // ----------------------------------------------------------------

        /**
         * @brief Встановлює режим для модуля.
         *
         * @param mode Може мати значення MODE_CONFIG або MODE_NORMAL.
         */
        void setMode(LRE220T_Mode mode);

        /**
         * @brief Повертає поточний режим роботи модуля.
         *
         * @return LRE220T_Mode
         */
        LRE220T_Mode getMode() const { return _mode; }

        // ----------------------------------------------------------------

        /**
         * @brief Встановлює номер каналу, на якому буде працювати модуль.
         * Цей метод змінює тільки локальне налаштування.
         * Для запису налаштувань в модуль, необхідно викликати writeSettings().
         *
         * @param channel Може мати значення від 0 - 80.
         */
        void setChannel(uint8_t channel);

        /**
         * @brief Повертає локальне значення налаштування номеру каналу.
         *
         * @return uint8_t
         */
        uint8_t getChannel() const { return _channel; }

        // ----------------------------------------------------------------

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

        // ----------------------------------------------------------------

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

        // ----------------------------------------------------------------

        /**
         * @brief Встановлює розмір пакета даних у байтах.
         * Цей метод змінює тільки локальне налаштування.
         * Для запису налаштувань в модуль, необхідно викликати writeSettings().
         *
         * @param len
         */
        void setPacketLen(PacketLen len);

        /**
         * @brief Повертає локальне значення налаштування розміру пакета даних із перечислення.
         *
         * @return PacketLen
         */
        PacketLen getPacketLen() const { return _packet_len; }

        /**
         * @brief Повертає локальне значення налаштування розміру пакета даних у байтах.
         *
         * @return uint8_t
         */
        uint8_t getRealPacketLen() const { return _real_packet_len; }

        // ----------------------------------------------------------------

        /**
         * @brief Надсилає пакет, що містить один байт даних.
         *
         */
        void warmUp();

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
         * @return false - Інакше.
         */
        bool readPacket(uint8_t *out_buff);

        /**
         * @brief Перевіряє наявність доступного пакету у буфері даних LoRa.
         *
         * @return true - Якщо у буфері є мінімум один пакет даних.
         * @return false - Інакше.
         */
        bool packetAvailable();

        /**
         * @brief Очищує усі вхідні дані із буфера LoRa.
         *
         */
        void flushBuffer();

    private:
        // ----------------------------------------------------------------

        struct REG_0
        {
            uint8_t addr_high{0};
        };

        struct REG_1
        {
            uint8_t addr_low{0};
        };

        struct REG_2
        {
            uint8_t air_data_rate : 3;
            uint8_t pairity_bit : 2;
            uint8_t serial_rate : 3;
        };

        struct REG_3
        {
            TransmitPower transmit_power : 2;
            uint8_t RESERVED : 3;
            uint8_t rssi_amb_en : 1;
            uint8_t packet_len : 2;
        };

        struct REG_4
        {
            uint8_t channel{0};
        };

        struct REG_5
        {
            uint8_t wor_cycle : 3;
            uint8_t RESERVED_2 : 1;
            uint8_t lbt_en : 1;
            uint8_t RESERVED_1 : 1;
            uint8_t transmit_meth : 1;
            uint8_t rssi_byte_en : 1;
        };

        struct REG_6
        {
            uint8_t crypt_key_h{0};
        };

        struct REG_7
        {
            uint8_t crypt_key_l{0};
        };

        // ----------------------------------------------------------------

        const uint32_t CONFIG_BAUD_RATE = 9600;
        const uint32_t NORMAL_BAUD_RATE = 115200;

        // ----------------------------------------------------------------

        TransmitPower _transmit_power{POWER_MAX};
        uint8_t _channel{23};
        bool _is_inited{false};
        AirDataRate _air_data_rate = AIR_DATA_RATE_2_4K;
        PacketLen _packet_len = PACK_LEN_200B;
        uint8_t _real_packet_len = 200;

        // ----------------------------------------------------------------

        LRE220T_Mode _mode;

        // ----------------------------------------------------------------

        uint8_t _shadow_registers[8];
        REG_0 *_reg_0 = (REG_0 *)&_shadow_registers[0];
        REG_1 *_reg_1 = (REG_1 *)&_shadow_registers[1];
        REG_2 *_reg_2 = (REG_2 *)&_shadow_registers[2];
        REG_3 *_reg_3 = (REG_3 *)&_shadow_registers[3];
        REG_4 *_reg_4 = (REG_4 *)&_shadow_registers[4];
        REG_5 *_reg_5 = (REG_5 *)&_shadow_registers[5];
        REG_6 *_reg_6 = (REG_6 *)&_shadow_registers[6];
        REG_7 *_reg_7 = (REG_7 *)&_shadow_registers[7];

        // ----------------------------------------------------------------

        bool isBusy();

        void setup();
        bool readRegisters();
        bool writeRegisters();
        bool waitResponce(size_t resp_size);
        void waitBusy();
    };
} // namespace meowui
