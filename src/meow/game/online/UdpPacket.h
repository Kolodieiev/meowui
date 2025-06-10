#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <AsyncUDP.h>
#include "../DataStream.h"

namespace meow
{
    class UdpPacket : public DataStream
    {
    public:
        /**
         * @brief Перечислення, що містить базовий тип пакета.
         *
         */
        enum PacketType : uint8_t
        {
            TYPE_DATA = 0,  // Пакет для обміну даними.
            TYPE_PING,      // Пакет для перевірки стану з'єднання.
            TYPE_HANDSHAKE, // Пакет для розпізнавання сервера.
            TYPE_NAME,      // Пакет з іменем клієнта.
            TYPE_BUSY,      // Пакет, що вказує на зайнятість сервера.
        };

        /**
         * @brief Створює новий об'єкт на основі даних об'єкта AsyncUDPPacket.
         *
         * @param packet
         */
        explicit UdpPacket(AsyncUDPPacket &packet);

        /**
         * @brief Створює новий об'єкт та виділяє місце під дані.
         *
         * @param data_len Розмір буфера даних.
         */
        explicit UdpPacket(size_t data_len);

        /**
         * @brief Встановлює тип пакета.
         *
         * @param type Тип пакета.
         */
        void setType(PacketType type) { _buffer[0] = type; }

        /**
         * @brief Повертає тип пакета.
         *
         * @return PacketType
         */
        PacketType getType() const { return static_cast<PacketType>(_buffer[0]); }

        /**
         * @brief Повертає вказівник із заданим зміщенням на дані пакета.
         * Дані за вказівником не повинні бути видалені або змінені.
         * В іншому випадку, поведінка програми невизначена.
         *
         * @param data_pos Зміщення відносно початку даних пакета.
         * @return const char* - Вказівник на дані з урахуванням зміщення.
         * Якщо зміщення перевищує кількість даних - вказівник на 0-вий байт даних.
         */
        const char *getData(uint16_t data_pos = 0) const;

        /**
         * @brief Повертає розмір даних пакета.
         *
         * @return size_t
         */
        size_t dataLen() const { return _size - 1; }

        /**
         * @brief Виводить дані пакета до UART.
         * Використовується для відлагодження.
         *
         * @param char_like Прапор, який вказує на формат виводу даних.
         * Якщо true кожен байт виводиться - як символ.
         * Інакше як hex-число.
         */
        void printToLog(bool char_like = true) const;

        /**
         * @brief Повертає віддалену ip-адресу, з якої було отримано цей пакет.
         * Значення буде не 0 тільки у випадку, якщо пакет було сконструйовано на основі даних об'єкта AsyncUDPPacket.
         *
         * @return IPAddress
         */
        IPAddress getRemoteIP() const { return _remote_ip; }

        /**
         * @brief Повертає порт, з якого було отримано цей пакет.
         * Значення буде не 0 тільки у випадку, якщо пакет було сконструйовано на основі даних об'єкта AsyncUDPPacket.
         *
         * @return uint16_t
         */
        uint16_t getRemotePort() const { return _port; }

        /**
         * @brief Порівнює побайтово дані з заданої позиції.
         *
         * @param data Буфер, з яким будуть порівнюватися дані пакета.
         * @param start_pos Зміщення в буфері даних пакета.
         * @param data_len Кількість байтів, які потрібно порівняти. Якщо 0 - порівняти до кінця даних пакета.
         * @return true - Якщо байти даних співпадають.
         * @return false - Інакше.
         */
        bool isDataEquals(const void *data, size_t start_pos = 0, size_t data_len = 0) const;

        /**
         * @brief Копіює вказану кількість байтів даних у зовнішній буфер.
         *
         * @param out Вказівник на зовнішній буфер, куди буде скопійовано байти даних.
         * @param start_pos Початкова позиція звідки необхідно почати копіювати дані.
         * @param len Кількість байтів, які необхідно скопіювати.
         * @return size_t - Кількість скопійованих байтів.
         */
        size_t extractBytes(void *out, size_t start_pos = 0, size_t len = 1) const;

        /**
         * @brief Скидає позицію каретки відносно даних.
         * Дані в буфері залишаються без змін.
         *
         */
        void flush() { _index = 1; }

        /**
         * @brief Повертає поточну позицію каретки в буфері відносно секції даних.
         *
         * @return size_t
         */
        size_t index() const { return _index - 1; }

    private:
        using DataStream::flush;
        using DataStream::index;

        UdpPacket() {}

    private:
        IPAddress _remote_ip;
        uint16_t _port{0};
    };
}