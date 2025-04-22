#include "DisplayUtil.h"

#ifdef BACKLIGHT_PIN

void meow::DisplayUtil::enableBackLight()
{
    pinMode(BACKLIGHT_PIN, OUTPUT);
    digitalWrite(BACKLIGHT_PIN, HIGH);
}

void meow::DisplayUtil::disableBackLight()
{
    digitalWrite(BACKLIGHT_PIN, LOW);
}

void meow::DisplayUtil::setBrightness(uint8_t value)
{
    _cur_brightness = value;
    ledcSetup(PWM_CHANEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(BACKLIGHT_PIN, PWM_CHANEL);
    ledcWrite(PWM_CHANEL, value);
}

#endif