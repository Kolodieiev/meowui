#pragma once
#include <Arduino.h>
#include <driver/i2s_std.h>

namespace meow
{
    class I2SOutManager
    {
    public:
        /**
         * @brief Ініціалізує 0-вий канал на шині I2S в режимі виводу.
         *
         * @param sample_rate Кількість вибірок за секунду.
         * @return true - Якщо ініціалізацію виконано успішно.
         * @return false - Інакше.
         */
        bool init(uint32_t sample_rate = 16000ul);

        /**
         * @brief Змінює частоту дискретизіації.
         * Автоматично вимикає канал перед зміною та вмикає після зміни частоти дескритизації.
         * Також самостійно обнуляє перші 128 байтів буферу I2S.
         *
         * @param sample_rate Кількість вибірок за секунду.
         */
        void reconfigSampleRate(uint32_t sample_rate);

        /**
         * @brief Set the I2SCommFMT LSB.
         *
         * @param comm_fmt false: I2S communication format is by default I2S_COMM_FORMAT_I2S_MSB, right->left (AC101, PCM5102A)
         * true:  changed to I2S_COMM_FORMAT_I2S_LSB for some DACs (PT8211)
         * Japanese or called LSBJ (Least Significant Bit Justified) format
         */
        void setI2SCommFMT_LSB(bool comm_fmt);

        /**
         * @brief Деініціалізує ініціалізований раніше канал на шині I2S.
         *
         */
        void deinit();

        /**
         * @brief Активує канал виводу аудіоданих на ініціалізованиій шині I2S.
         *
         * @return esp_err_t
         */
        esp_err_t enable();

        /**
         * @brief Деактивує канал виводу аудіоданих на ініціалізованиій шині I2S.
         *
         */
        void disable();

        /**
         * @brief Очищує перші 128 байтів буферу каналу аудіовиходу.
         *
         */
        void clearBuffer();

        /**
         * @brief Копіює дані із зовнішнього буфера в буфер шини I2S.
         *
         * @param buffer Зовнішній буфер з даними.
         * @param buff_len Розмір зовнішнього буфера.
         * @param only_left_chan Прапор, який вказує на тип аудіоданих у буфері.
         * Якщо true - дані перед відтворенням будуть розширені по схемі left_chann -> stereo.
         * Якщо false - дані будуть відтворені в стерео режимі як є.
         * @return size_t - Кількість успішно скопійованих семплів.
         */
        size_t write(const int16_t *buffer, size_t buff_len, bool only_left_chan = false);

        /**
         * @brief Повертає значення прапору, який вказує на стан ініціалізації I2S-каналу аудіовиходу.
         *
         * @return true - Якщо канал було успішно ініціалізовано.
         * @return false - Інакше.
         */
        bool isInited() const;

    private:
        static bool _is_inited;

        //--------------------------------------------

        i2s_std_config_t _i2s_tx_std_cfg{};
        i2s_chan_config_t _i2s_chan_cfg{};
        i2s_chan_handle_t _i2s_tx_handle{};

        bool _comm_fmt{false};
    };

    /**
     * @brief Глобальний об'єкт-обгортка для виводу аудіо по шині I2S.
     *
     */
    extern I2SOutManager _i2s_out;
}