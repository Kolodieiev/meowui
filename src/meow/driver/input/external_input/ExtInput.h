#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "meowui_setup/input_setup.h"
#include "meow/manager/CoprocessorManager.h"

#ifdef EXT_INPUT
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
        bool getBtnState(uint8_t btn_pos) const;
        void enableBtn(uint8_t btn_pos);
        void disableBtn(uint8_t btn_pos);

    private:
        uint8_t _buttons_state[EXT_INPUT_B_NUM]{0};
    };
}

#endif // EXT_INPUT
