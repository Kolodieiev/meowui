#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>

void downsampleX2(const int16_t *in_buff, int16_t *out_buff, size_t in_size);
void upsampleX2(const int16_t *in_buff, int16_t *out_buff, size_t in_size);
void volume(int16_t *in_buff, size_t in_size, int16_t gain_db);

class HighPassFilter
{
public:
    HighPassFilter();
    void filter(int16_t *buffer, size_t buffer_size);
    void init(float cutoff_freq = 200.0f, uint32_t sample_rate = 16000);

private:
    float x1 = 0.0f, x2 = 0.0f, y1 = 0.0f, y2 = 0.0f;
    float a0{0}, a1{0}, a2{0}, b1{0}, b2{0};
};

class SimpleAGC
{
public:
    SimpleAGC(float target_dB = -7.0f, float attack_rate = 0.2f, float release_rate = 0.1f)
        : _target_level(target_dB), _attack(attack_rate), _release(release_rate), _gain(1.0f) {}

    void process(int16_t *buffer, size_t size);
    void setTargetDB(float target_dB = -7.0f);

private:
    float _target_level; // Цільовий рівень в dB
    float _attack;       // Швидкість зменшення гейну
    float _release;      // Швидкість збільшення гейну
    float _gain;         // Поточний коефіцієнт гейну
};
