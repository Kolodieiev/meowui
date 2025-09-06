#pragma GCC optimize("O3")
#include "ExtInput.h"
#include "meow/manager/CoprocessorManager.h"

#ifdef EXT_INPUT

#define RESP_PREP_TIME 1

namespace meow
{
    void ExtInput::init()
    {
        _ccpu.connect();
    }

    void ExtInput::update()
    {
        CoprocessorCMD_t cmd = CCPU_CMD_GET_BTNS_STATE;

        if (!_ccpu.sendCmd(&cmd, sizeof(cmd), RESP_PREP_TIME))
            return;

        if (!_ccpu.readData(_buttons_state, EXT_INPUT_B_NUM))
        {
            for (size_t i = 0; i < EXT_INPUT_B_NUM; ++i)
                _buttons_state[i] = 0;
        }
    }

    bool ExtInput::getBtnState(uint8_t btn_pos)
    {
        uint8_t byte_index = btn_pos / 8;

        if (byte_index != 0 && byte_index >= EXT_INPUT_B_NUM)
            return false;

        uint8_t bit_mask = 1 << (7 - (btn_pos % 8));

        return (_buttons_state[byte_index] & bit_mask) != 0;
    }

    void ExtInput::enableBtn(uint8_t btn_pos)
    {
        uint8_t cmd_data[2] = {CCPU_CMD_BTN_ON, btn_pos};
        _ccpu.sendCmd(cmd_data, sizeof(cmd_data));
    }

    void ExtInput::disableBtn(uint8_t btn_pos)
    {
        uint8_t cmd_data[2] = {CCPU_CMD_BTN_OFF, btn_pos};
        _ccpu.sendCmd(cmd_data, sizeof(cmd_data));
    }
}

#endif // EXT_INPUT