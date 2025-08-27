#pragma once
#include <Arduino.h>
#include <driver/i2s_std.h>

namespace meow
{
    class I2SInManager
    {
    public:
        /**
         * @brief Ініціалізує 1-ший канал на шині I2S в режимі ввводу.
         *
         * @param sample_rate Кількість вибірок за секунду.
         * @param has_interleaving Прапор, який вказує на формат даних, що повертає мікрофон.
         * Якщо false - семпли з мікрофону для одного каналу йдуть один за одним. Інакше - семпли чергуються для кожного каналу.
         * @return true - Якщо ініціалізацію виконано успішно.
         * @return false - Інакше.
         */
        bool init(uint32_t sample_rate = 16000ul, bool has_interleaving = false);

        /**
         * @brief Деініціалізує ініціалізований раніше канал на шині I2S.
         *
         */
        void deinit();

        /**
         * @brief Активує канал зчитування аудіоданих на ініціалізованиій шині I2S.
         *
         * @return esp_err_t
         */
        esp_err_t enable();

        /**
         * @brief Деактивує канал зчитування аудіоданих на ініціалізованиій шині I2S.
         *
         */
        void disable();

        /**
         * @brief Змінює частоту дискретизіації.
         * Автоматично вимикає канал перед зміною та вмикає після зміни частоти дескритизації.
         * Також самостійно обнуляє перші 128 байтів буферу I2S.
         *
         * @param sample_rate Кількість вибірок за секунду.
         */
        void reconfigSampleRate(uint32_t sample_rate);

        /**
         * @brief Читає дані з буфера раніше налаштованого I2S-каналу.
         *
         * @param out_buffer Буфер, куди будуть прочитані дані.
         * @param buff_len Довжина буфера.
         * @return size_t - Кількість прочитаних семплів.
         */
        size_t read(int16_t *out_buffer, size_t buff_len);

        /**
         * @brief Очищує перші 128 байтів буферу каналу аудіовиходу.
         *
         */
        void clearBuffer();

        /**
         * @brief Повертає значення прапору, який вказує на стан ініціалізації I2S-каналу аудіовходу.
         *
         * @return true - Якщо канал було успішно ініціалізовано.
         * @return false - Інакше.
         */
        bool isInited() const;

    private:
        static bool _is_inited;

        i2s_std_config_t _i2s_rx_std_cfg{};
        i2s_chan_config_t _i2s_chan_cfg{};
        i2s_chan_handle_t _i2s_rx_handle{};

        bool _has_interleaving{false};
    };

    /**
     * @brief Глобальний об'єкт-обгортка для зчитування аудіо-даних по шині I2S.
     *
     */
    extern I2SInManager _i2s_in;
}