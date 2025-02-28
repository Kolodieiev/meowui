#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <driver/adc.h>

namespace meow
{
    class AnalogMicrophone
    {
    public:
        ~AnalogMicrophone();

        /**
         * @brief Ініціалізує ADC_1 модуль і таймер для зчитування аналогового сигналу з відповідного каналу.
         *
         * @param channel Канал, на якому буде оцифровуватися сигнал.
         * @param out_samps_buff Вихідний буфер, куди будуть записуватися дані.
         * @param frame_size Розмір фрейма.
         * @param sample_rate Кількість вибірок за секунду.
         * @return true - Якщо ініціалізацію виконано успішно.
         * @return false - Інакше.
         */
        bool initADC_1(adc1_channel_t channel, int16_t *out_samps_buff, uint16_t frame_size, uint16_t sample_rate = 16000);

        /**
         * @brief Ініціалізує ADC_2 модуль і таймер для зчитування аналогового сигналу з відповідного каналу.
         *
         * @param channel Канал, на якому буде оцифровуватися сигнал.
         * @param out_samps_buff Вихідний буфер, куди будуть записуватися дані.
         * @param frame_size Розмір фрейма.
         * @param sample_rate Кількість вибірок за секунду.
         * @return true - Якщо ініціалізацію виконано успішно.
         * @return false - Інакше.
         */
        bool initADC_2(adc2_channel_t channel, int16_t *out_samps_buff, uint16_t frame_size, uint16_t sample_rate = 16000);

        /**
         * @brief Деініціалізує ініціалізований раніше модуль ADC.
         *
         */
        void deinit();

        /**
         * @brief Встановлює величину підсилення оцифрованого сигналу.
         *
         * @param gain Значення у скільки разів буде підсилено сигнал.
         */
        void setGainFactor(uint8_t gain) { _gain = gain; }

        /**
         * @brief Блокує доступ до зовнішнього буфера.
         *
         */
        void lock() { portENTER_CRITICAL_ISR(&_adc_mux); }

        /**
         * @brief Знімає блокування доступу до зовнішнього буфефра, та скидає лічильник поточної вибірки.
         *
         */
        void unlockAndFlush()
        {
            _frame_index = 0;
            portEXIT_CRITICAL_ISR(&_adc_mux);
        }

        /**
         * @brief Перевіряє чи заповнено буфер даних.
         *
         * @return true - Якщо буфер заповнено.
         * @return false - Інакше.
         */
        bool isFull() const { return _frame_index == _frame_size; }

    private:
        static AnalogMicrophone *_instance;
        portMUX_TYPE _adc_mux = portMUX_INITIALIZER_UNLOCKED;
        uint8_t _gain = 1;
        volatile uint16_t _frame_index;
        adc1_channel_t _adc_chann_1;
        adc2_channel_t _adc_chann_2;
        esp_timer_handle_t _adc_timer;
        int16_t *_samps_buff;
        uint16_t _frame_size;

        static void adc1_timer_callback(void *arg);
        void adc1_timer_handler();
        static void adc2_timer_callback(void *arg);
        void adc2_timer_handler();
        //
        void checkInst();
    };
}