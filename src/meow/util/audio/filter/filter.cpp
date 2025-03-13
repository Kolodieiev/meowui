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

void volume(int16_t *in_buff, size_t in_size, int16_t gain)
{
    for (size_t i = 0; i < in_size; ++i)
        in_buff[i] *= gain;
}

void HighPassFilter::init(float cutoff_freq, uint32_t sample_rate)
{
    float omega_c = 2.0f * M_PI * cutoff_freq / sample_rate;
    float cos_omega = cosf(omega_c);
    float sin_omega = sinf(omega_c);
    float alpha = sin_omega / (1.0f + cos_omega);

    a0 = (1.0f + cos_omega) * 0.5f;
    a1 = -(1.0f + cos_omega);
    a2 = (1.0f + cos_omega) * 0.5f;

    b1 = -cos_omega;
    b2 = alpha;

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

void AutoGainControl::filter(int16_t *buffer, size_t buff_size, int16_t max_gain_db)
{
    constexpr float max_lvl = 32767.0f;
    float max_gain = powf(10.0f, max_gain_db * 0.05f);

    for (size_t i = 0; i < buff_size; i++)
    {
        float x = static_cast<float>(buffer[i]);
        float abs_x = fabsf(x);

        float tmp_gain = (abs_x > 1.0f) ? (max_lvl / abs_x) : max_gain;

        curr_gain += (tmp_gain - curr_gain) * 0.05f;
        curr_gain = std::min(curr_gain, max_gain);

        float tmp_val = x * curr_gain;

        if (tmp_val > max_lvl)
            buffer[i] = static_cast<int16_t>(max_lvl);
        else if (tmp_val < -max_lvl)
            buffer[i] = static_cast<int16_t>(-max_lvl);
        else
            buffer[i] = static_cast<int16_t>(tmp_val);
    }
}
