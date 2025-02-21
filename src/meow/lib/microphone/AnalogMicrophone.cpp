#pragma GCC optimize("O3")
#include "AnalogMicrophone.h"
#include <WiFi.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

const char ADC1_TIMER_NAME[] = "adc1";
const char ADC2_TIMER_NAME[] = "adc2";

namespace meow
{
    AnalogMicrophone *AnalogMicrophone::_instance{nullptr};

    AnalogMicrophone::~AnalogMicrophone()
    {
        deinit();
    }

    bool AnalogMicrophone::initADC_1(adc1_channel_t channel, int16_t *out_samps_buff, uint16_t buff_size, uint16_t sample_rate)
    {
        checkInst();

        _adc_chann_1 = channel;
        _samps_buff = out_samps_buff;
        _buff_size = buff_size;

        bool result = false;

        result |= adc1_config_width(ADC_WIDTH_BIT_12);
        result |= adc1_config_channel_atten(_adc_chann_1, ADC_ATTEN_DB_12);

        const esp_timer_create_args_t adc_timer_args = {
            .callback = &adc1_timer_callback,
            .name = ADC1_TIMER_NAME};

        result |= esp_timer_create(&adc_timer_args, &_adc_timer);
        result |= esp_timer_start_periodic(_adc_timer, 1000000 / sample_rate);

        if (result)
            log_e("Помилка ініціалізації adc1");
        return !result;
    }

    bool AnalogMicrophone::initADC_2(adc2_channel_t channel, int16_t *out_samps_buff, uint16_t buff_size, uint16_t sample_rate)
    {
        if (WiFi.getMode() != WIFI_OFF)
        {
            log_e("ADC_2 не може працювати коректно разом з WiFi");
            esp_restart();
        }

        checkInst();

        _adc_chann_2 = channel;
        _samps_buff = out_samps_buff;
        _buff_size = buff_size;

        bool result = false;
        result |= adc2_config_channel_atten(_adc_chann_2, ADC_ATTEN_DB_12);

        const esp_timer_create_args_t adc_timer_args = {
            .callback = &adc2_timer_callback,
            .name = ADC2_TIMER_NAME};

        result |= esp_timer_create(&adc_timer_args, &_adc_timer);
        result |= esp_timer_start_periodic(_adc_timer, 1000000 / sample_rate);

        if (result)
            log_e("Помилка ініціалізації adc2");
        return !result;
    }

    void AnalogMicrophone::deinit()
    {
        if (!_instance)
            return;
        esp_timer_stop(_adc_timer);
        esp_timer_delete(_adc_timer);
        _instance = nullptr;
    }

    void AnalogMicrophone::adc1_timer_callback(void *arg)
    {
        _instance->adc1_timer_handler();
    }

    void AnalogMicrophone::adc1_timer_handler()
    {
        portENTER_CRITICAL_ISR(&_adc_mux);
        if (_buff_index < _buff_size)
        {
            int sample = adc1_get_raw(_adc_chann_1);
            int amplified_value = sample * _gain;
            int16_t clamped_value = constrain(amplified_value, INT16_MIN, INT16_MAX);
            _samps_buff[_buff_index++] = clamped_value;
        }
        portEXIT_CRITICAL_ISR(&_adc_mux);
    }

    void AnalogMicrophone::adc2_timer_callback(void *arg)
    {
        _instance->adc2_timer_handler();
    }

    void AnalogMicrophone::adc2_timer_handler()
    {
        portENTER_CRITICAL_ISR(&_adc_mux);
        if (_buff_index < _buff_size)
        {
            int sample;
            adc2_get_raw(_adc_chann_2, ADC_WIDTH_BIT_12, &sample);
            int amplified_value = sample * _gain;
            int16_t clamped_value = constrain(amplified_value, INT16_MIN, INT16_MAX);
            _samps_buff[_buff_index++] = clamped_value;
        }
        portEXIT_CRITICAL_ISR(&_adc_mux);
    }

    void AnalogMicrophone::checkInst()
    {
        if (_instance)
        {
            log_e("Підтримується тільки один мікрофон");
            esp_restart();
        }

        _instance = this;
    }
}