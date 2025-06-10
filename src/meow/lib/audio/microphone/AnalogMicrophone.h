#pragma once
#include <Arduino.h>
#include <esp_adc/adc_continuous.h>

namespace meow
{
    class AnalogMicrophone
    {
    public:
        AnalogMicrophone() {}
        ~AnalogMicrophone();

        /**
         * @brief Ініціалізує аналоговий мікрофон.
         *
         * @param adc_unit На рівні esp-idf підтримується тільки UNIT_1
         * @param adc_chann Канал (не пін).
         * @param out_samps_buff Вихідний буфер.
         * @param buff_size Розмір вихідного буфера.
         * @param sample_rate Частота дискретизації.
         * @return true - Якщо ініціалізація відбулась успішно.
         * @return false - Інакше.
         */
        bool init(adc_unit_t adc_unit, adc_channel_t adc_chann, int16_t *out_samps_buff, uint32_t buff_size, uint32_t sample_rate = 16000);

        /**
         * @brief Деініціалізує аналоговий мікрофон, звільняючи ресурси.
         */
        void deinit();

        /**
         * @brief Перевіряє, чи заповнений внутрішній буфер семплів.
         * @return true - якщо буфер заповнений, false - інакше.
         */
        bool isDataFull() const { return _frame_index >= _buff_size; }

        /**
         * @brief Скидає індекс буфера, дозволяючи перезапис.
         */
        void resetFrameIndex()
        {
            _frame_index = 0;
        }

        /**
         * @brief Встановлює коефіцієнт підсилення (для обробки семплів, не впливає на ADC).
         * @param gain Коефіцієнт підсилення.
         */
        void setGainFactor(uint8_t gain) { _gain = gain; }

    private:
        static bool convDoneCb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data);
        static void readAnalogMicroTask(void *arg);

    private:
        TaskHandle_t _read_task_handle{nullptr};
        adc_continuous_handle_t _adc_handle{nullptr};

        int16_t *_samps_buff{nullptr};
        uint8_t *_dma_read_buffer{nullptr};

        adc_unit_t _adc_unit;
        adc_channel_t _adc_channel;

        uint32_t _sample_rate{0};
        uint32_t _dma_buffer_actual_size{0};

        uint16_t _frame_index{0};
        uint16_t _buff_size{0};

        uint8_t _gain{1};

        bool _is_inited{false};
    };
}
