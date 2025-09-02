#pragma GCC optimize("O3")
#include "filter.h"

void downsampleX2(const int16_t *in_buff, int16_t *out_buff, size_t in_size)
{
    const int16_t *in_end = in_buff + in_size;

    while (in_buff < in_end)
    {
        *out_buff++ = (*in_buff + *(in_buff + 1)) >> 1;
        in_buff += 2;
    }
}

void upsampleX2(const int16_t *in_buff, int16_t *out_buff, size_t in_size)
{
    const int16_t *in_end = in_buff + in_size;

    while (in_buff < in_end)
    {
        *out_buff++ = *in_buff;
        *out_buff++ = *in_buff++;
    }
}

void volume(int16_t *in_buff, size_t in_size, int16_t gain_db)
{
    float gain = powf(10.0f, gain_db * 0.05f);
    constexpr float max_lvl = 32767.0f;

    for (size_t i = 0; i < in_size; ++i)
    {
        float x = static_cast<float>(in_buff[i]) * gain;

        if (x > max_lvl)
            in_buff[i] = static_cast<int16_t>(max_lvl);
        else if (x < -max_lvl)
            in_buff[i] = static_cast<int16_t>(-max_lvl);
        else
            in_buff[i] = static_cast<int16_t>(x);
    }
}

HighPassFilter::HighPassFilter()
{
    init();
}

void HighPassFilter::init(float cutoff_freq, uint32_t sample_rate)
{
    float omega = 2.0f * M_PI * cutoff_freq / sample_rate;
    float cos_omega = cosf(omega);
    float sin_omega = sinf(omega);
    float Q = 0.707f; // Чистий другий порядок (Butterworth)
    float alpha = sin_omega / (2.0f * Q);

    float b0 = (1.0f + cos_omega) * 0.5f;
    float b1 = -(1.0f + cos_omega);
    float b2 = (1.0f + cos_omega) * 0.5f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cos_omega;
    float a2 = 1.0f - alpha;

    // нормалізація
    this->a0 = b0 / a0;
    this->a1 = b1 / a0;
    this->a2 = b2 / a0;
    this->b1 = a1 / a0;
    this->b2 = a2 / a0;

    x1 = x2 = y1 = y2 = 0.0f;
}

void HighPassFilter::filter(int16_t *buffer, size_t buffer_size)
{
    for (int i = 0; i < buffer_size; ++i)
    {
        float x0 = (float)buffer[i];

        float y0 = a0 * x0 + a1 * x1 + a2 * x2 - b1 * y1 - b2 * y2;

        buffer[i] = (int16_t)y0;

        x2 = x1;
        x1 = x0;
        y2 = y1;
        y1 = y0;
    }
}

void SimpleAGC::process(int16_t *buffer, size_t size)
{
    // Обчислення RMS для поточного кадру
    float rms = 0.0f;
    for (size_t i = 0; i < size; ++i)
        rms += buffer[i] * buffer[i];

    rms = sqrtf(rms / size);

    // Перетворення RMS в dB
    float rms_dB = 20.0f * log10f(rms / 32768.0f);

    // Розрахунок необхідного коефіцієнта гейну
    float target_gain = powf(10.0f, (_target_level - rms_dB) / 20.0f);

    // Плавне коригування гейну
    if (rms_dB > _target_level)
        _gain *= (1.0f - _attack);
    else
        _gain *= (1.0f + _release);

    // Обмеження гейну
    if (_gain > 1.0f)
        _gain = 1.0f;
    if (_gain < 0.0f)
        _gain = 0.0f;

    // Застосування гейну до аудіо буфера
    for (size_t i = 0; i < size; ++i)
    {
        int32_t temp = static_cast<int32_t>(buffer[i]) * _gain;
        if (temp > 32767)
            buffer[i] = 32767;
        else if (temp < -32768)
            buffer[i] = -32768;
        else
            buffer[i] = static_cast<int16_t>(temp);
    }
}

#define MIN_AGC_DB -15.0f
#define MAX_AGC_DB 0.0f

void SimpleAGC::setTargetDB(float target_dB)
{
    if (target_dB < MIN_AGC_DB)
        target_dB = MIN_AGC_DB;
    else if (target_dB > MAX_AGC_DB)
        target_dB = MAX_AGC_DB;

    _target_level = target_dB;
}
