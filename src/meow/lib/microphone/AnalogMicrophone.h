#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include "driver/adc.h"

namespace meow
{
    class AnalogMicrophone
    {
    public:
        ~AnalogMicrophone();

        bool initADC_1(adc1_channel_t channel, int16_t *out_samps_buff, uint16_t buff_size, uint16_t sample_rate = 16000);
        bool initADC_2(adc2_channel_t channel, int16_t *out_samps_buff, uint16_t buff_size, uint16_t sample_rate = 16000);
        void deinit();

        void setGainFactor(uint8_t gain) { _gain = gain; }

        void lock() { portENTER_CRITICAL_ISR(&_adc_mux); }
        void unlockAndFlush()
        {
            _buff_index = 0;
            portEXIT_CRITICAL_ISR(&_adc_mux);
        }
        bool isFull() const { return _buff_index == _buff_size; }

    private:
        static AnalogMicrophone *_instance;
        portMUX_TYPE _adc_mux = portMUX_INITIALIZER_UNLOCKED;
        uint8_t _gain = 1;
        volatile uint16_t _buff_index;
        adc1_channel_t _adc_chann_1;
        adc2_channel_t _adc_chann_2;
        esp_timer_handle_t _adc_timer;
        int16_t *_samps_buff;
        uint16_t _buff_size;

        static void adc1_timer_callback(void *arg);
        void adc1_timer_handler();
        static void adc2_timer_callback(void *arg);
        void adc2_timer_handler();
        //
        void checkInst();
    };
}