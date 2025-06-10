#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>

namespace meow
{
    class DataStream
    {
    public:
        /**
         * @brief Конструює новий DataStream об'єкт.
         *
         * @param size Розмір буфера даних.
         */
        explicit DataStream(size_t size);
        ~DataStream();

        /**
         * @brief Змінює розмір буфера даних.
         * Старі дані буде втрачено.
         *
         * @param size
         */
        void resize(size_t size);

        /**
         * @brief Копіює вказану кількість байтів до зовнішнього буфера.
         * Переміщує початок буфера на скопійовану кількість байтів.
         *
         * @param out Вказівник на зовнішній буфер.
         * @param len Кількість байтів, які необхідно скопіювати.
         * @return size_t - Кількість скопійованих байтів.
         */
        size_t read(void *out, size_t len = 1);

        /**
         * @brief Переміщує початок буфера на вказану кількість байтів.
         *
         * @param len Кількість байтів, які необхідно пропустити.
         * @return size_t Кількість пропущених байтів.
         */
        size_t drop(size_t len = 1);

        /**
         * @brief Копіює вказану кількість байтів до внутрішнього буфера.
         *
         * @param data Масив даних, звідки будуть скопійовані дані.
         * @param len Кількість даних, які необхідно скопіювати.
         * @return size_t - Кількість скопійованих байтів.
         */
        size_t write(const void *data, size_t len = 1);

        /**
         * @brief Копіює байти об'єкта до внутрішнього буфера.
         *
         * @tparam T Тип об'єкта.
         * @param data Об'єкт, який буде скопійовано.
         * @return size_t - Кількість скопійованих байтів.
         */
        template <typename T>
        size_t write(const T &data)
        {
            return write(&data, sizeof(T));
        }

        /**
         * @brief Повертає кількість вільного місця в буфері, що доступне для запису.
         *
         * @return size_t
         */
        size_t space() const;

        /**
         * @brief Повертає загальний розмір буфера.
         *
         * @return size_t
         */
        size_t length() const { return _size; }

        /**
         * @brief Повертає поточну позицію каретки в буфері.
         *
         * @return size_t
         */
        size_t index() const { return _index; }

        /**
         * @brief Скидає позицію каретки.
         * Дані в буфері залишаються без змін.
         *
         */
        void flush() { _index = 0; }

        /**
         * @brief Повертає вказівник на буфер з даними.
         * Буфер за цим вказівником не повинен бути змінений або видалений.
         * Невиконання цієї умови призведе до невизначеної поведінки.
         * "Сирий" вказівник на буфер потрібний в деяких випадках, щоб уникнути копіювання даних.
         *
         * @return uint8_t*
         */
        const uint8_t *raw() const { return _buffer; }

        operator bool()
        {
            return _buffer != nullptr;
        }

    protected:
        DataStream() {}

    protected:
        uint8_t *_buffer{nullptr};
        size_t _index;
        size_t _size;
    };
}