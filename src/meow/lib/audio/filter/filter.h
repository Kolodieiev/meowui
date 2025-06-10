#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>

void downsampleX2(const int16_t *in_buff, int16_t *out_buff, size_t in_size);
void upsampleX2(const int16_t *in_buff, int16_t *out_buff, size_t in_size);
void volume(int16_t *in_buff, size_t in_size, int16_t gain);


class HighPassFilter
{
public:
    void init(float cutoff_freq, uint32_t sample_rate);
    void filter(int16_t *buffer, size_t buffer_size);

private:
    float x1 = 0.0f, x2 = 0.0f, y1 = 0.0f, y2 = 0.0f;
    float a0{0}, a1{0}, a2{0}, b1{0}, b2{0};
};

class AutoGainControl
{
public:
    void filter(int16_t *buffer, size_t buff_size, int16_t max_gain_db);

private:
    float MAX_16BIT_LVL = 32767.0f;
    float curr_gain = 1.0f;
};