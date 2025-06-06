#include "AnalogMicrophone.h"
#include "meow/manager/wifi/WiFiManager.h"

#define ADC_DMA_BUFFER_FACTOR 2

namespace meow
{
    AnalogMicrophone::~AnalogMicrophone()
    {
        deinit();
    }

    bool AnalogMicrophone::init(adc_unit_t adc_unit, adc_channel_t adc_chann, int16_t *out_samps_buff, uint32_t buff_size, uint32_t sample_rate)
    {
        if (_is_inited)
            return true;

        if (!out_samps_buff || buff_size == 0)
        {
            log_e("Недійсний вихідний буфер або розмір буфера");
            esp_restart();
        }

        if (adc_unit == ADC_UNIT_2 && _wifi.isEnabled())
        {
            log_e("ADC_UNIT_2 не може працювати одночасно з WiFi");
            return false;
        }

        _adc_unit = adc_unit;
        _adc_channel = adc_chann;
        _sample_rate = sample_rate;
        _samps_buff = out_samps_buff;
        _buff_size = buff_size;
        _frame_index = 0;

        _dma_buffer_actual_size = _buff_size * SOC_ADC_DIGI_RESULT_BYTES * ADC_DMA_BUFFER_FACTOR;

        _dma_read_buffer = (uint8_t *)heap_caps_malloc(_dma_buffer_actual_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        if (!_dma_read_buffer)
        {
            log_e("Не вдалося виділити DMA буфер");
            return false;
        }

        //

        adc_continuous_handle_cfg_t adc_config = {
            .max_store_buf_size = _dma_buffer_actual_size,
            .conv_frame_size = 256,
        };

        if (adc_continuous_new_handle(&adc_config, &_adc_handle))
        {
            log_e("Не вдалося створити хендл ADC continuous");
            deinit();
            return false;
        }

        //

        adc_digi_pattern_config_t pattern_config[] = {
            {
                .atten = ADC_ATTEN_DB_12,
                .channel = _adc_channel,
                .unit = _adc_unit,
                .bit_width = SOC_ADC_DIGI_MAX_BITWIDTH,
            },
        };

        adc_continuous_config_t dig_cfg = {
            .pattern_num = sizeof(pattern_config) / sizeof(pattern_config[0]),
            .adc_pattern = pattern_config,
            .sample_freq_hz = _sample_rate,
            .conv_mode = (_adc_unit == ADC_UNIT_1) ? ADC_CONV_SINGLE_UNIT_1 : ADC_CONV_SINGLE_UNIT_2,
            .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
        };

        if (adc_continuous_config(_adc_handle, &dig_cfg) != ESP_OK)
        {
            log_e("Не вдалося налаштувати ADC continuous");
            deinit();
            return false;
        }

        //

        adc_continuous_evt_cbs_t cbs = {
            .on_conv_done = convDoneCb,
            .on_pool_ovf = NULL,
        };

        if (adc_continuous_register_event_callbacks(_adc_handle, &cbs, this) != ESP_OK)
        {
            log_e("Не вдалося зареєструвати колбеки подій ADC");
            deinit();
            return false;
        }

        //

        xTaskCreate(readAnalogMicroTask, "AnalogMicroTask", 4096, this, 5, &_read_task_handle);
        if (_read_task_handle == nullptr)
        {
            log_e("Не вдалося створити задачу зчитування мікрофона");
            deinit();
            return false;
        }

        //

        if (adc_continuous_start(_adc_handle) != ESP_OK)
        {
            log_e("Не вдалося запустити ADC continuous");
            deinit();
            return false;
        }

        _is_inited = true;
        return true;
    }

    void AnalogMicrophone::deinit()
    {
        if (!_is_inited)
            return;

        if (_read_task_handle)
        {
            vTaskDelete(_read_task_handle);
            _read_task_handle = nullptr;
        }

        if (_adc_handle)
        {
            adc_continuous_stop(_adc_handle);
            adc_continuous_deinit(_adc_handle);
            _adc_handle = nullptr;
        }

        if (_dma_read_buffer)
        {
            free(_dma_read_buffer);
            _dma_read_buffer = nullptr;
            _dma_buffer_actual_size = 0;
        }

        _is_inited = false;
        _frame_index = 0;
        log_i("AnalogMicrophone деініціалізовано");
    }

    bool AnalogMicrophone::convDoneCb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
    {
        BaseType_t ret_task_woken = pdFALSE;
        AnalogMicrophone *self = static_cast<AnalogMicrophone *>(user_data);

        if (self && self->_read_task_handle != nullptr)
            vTaskNotifyGiveFromISR(self->_read_task_handle, &ret_task_woken);

        return (ret_task_woken == pdTRUE);
    }

    void AnalogMicrophone::readAnalogMicroTask(void *arg)
    {
        AnalogMicrophone *self = static_cast<AnalogMicrophone *>(arg);
        if (!self || !self->_adc_handle || !self->_dma_read_buffer)
        {
            log_e("Недійсний аргумент або неініціалізований ADC/DMA буфер");
            esp_restart();
        }

        uint32_t bytes_read;
        esp_err_t ret;

        while (1)
        {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

            ret = adc_continuous_read(self->_adc_handle, self->_dma_read_buffer, self->_dma_buffer_actual_size, &bytes_read, 0);

            if (ret != ESP_OK)
            {
                log_e("Не вдалося зчитати дані АЦП: %s", esp_err_to_name(ret));
            }
            else
            {
                for (int i = 0; i < bytes_read; i += SOC_ADC_DIGI_RESULT_BYTES)
                {
                    if (self->_frame_index >= self->_buff_size)
                        break;

                    adc_digi_output_data_t *p = (adc_digi_output_data_t *)&self->_dma_read_buffer[i];

#if CONFIG_IDF_TARGET_ESP32
                    uint16_t adc_raw = p->type1.data;
#elif CONFIG_IDF_TARGET_ESP32S3
                    uint16_t adc_raw = p->type2.data;
#endif
                    self->_samps_buff[self->_frame_index] = (int16_t)(((int32_t)adc_raw - 2048) * self->_gain);
                    ++self->_frame_index;
                }
            }
        }
    }
}