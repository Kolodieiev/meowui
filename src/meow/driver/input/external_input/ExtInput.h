#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "meowui_setup/input_setup.h"
#include "meow/manager/i2c/I2C_Manager.h"

#ifdef EXT_INPUT
#if !defined(EXT_INPUT_ADDR) || (EXT_INPUT_ADDR < 1) || (EXT_INPUT_ADDR > 126)
#error "EXT_INPUT_ADDR не задано, або значення не є коректним."
#endif // #if !defined(EXT_INPUT_ADDR)
#if !defined(EXT_INPUT_B_NUM) || (EXT_INPUT_B_NUM < 1)
#error "EXT_INPUT_B_NUM не задано, або значення не є коректним."
#endif // !defined(EXT_INPUT_B_NUM)

namespace meow
{
    class ExtInput
    {
    public:
        void init();
        void update();
        bool getBtnState(uint8_t btn_pos);
        void enableBtn(uint8_t btn_pos);
        void disableBtn(uint8_t btn_pos);

    private:
        I2C_Manager _i2c;
        uint8_t _buttons_state[EXT_INPUT_B_NUM]{0};

        void sendBtnCmd(const void *buff, size_t buff_size);
    };
}

#endif // EXT_INPUT
