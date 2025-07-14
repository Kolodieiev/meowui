#pragma GCC optimize("O3")
#include "ExtInput.h"

#ifdef EXT_INPUT

#define RESP_PREP_TIME 1

namespace meow
{
    void ExtInput::init()
    {
        _i2c.begin();
    }

    void ExtInput::update()
    {
        if (!_i2c.isInited())
            return;

        ExtI2CMD_t cmd = I2C_CMD_BTNS_STATE;
        if (_i2c.write(EXT_INPUT_ADDR, &cmd, sizeof(cmd)))
        {
            vTaskDelay(RESP_PREP_TIME / portTICK_PERIOD_MS);
            if (_i2c.read(EXT_INPUT_ADDR, _buttons_state, EXT_INPUT_B_NUM))
                return;
        }

        for (size_t i = 0; i < EXT_INPUT_B_NUM; ++i)
            _buttons_state[i] = 0;

        log_e("Пристрій вводу не відповідає коректно на I2C команди");
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
        uint8_t cmd_buff[2] = {I2C_CMD_ON_BTN, btn_pos};
        sendBtnCmd(cmd_buff, sizeof(cmd_buff));
    }

    void ExtInput::disableBtn(uint8_t btn_pos)
    {
        uint8_t cmd_buff[2] = {I2C_CMD_OFF_BTN, btn_pos};
        sendBtnCmd(cmd_buff, sizeof(cmd_buff));
    }

    void ExtInput::sendBtnCmd(const void *buff, size_t buff_size)
    {
        if (!_i2c.write(EXT_INPUT_ADDR, buff, buff_size))
            log_e("Пристрій вводу не відповідає коректно на I2C команди");
    }
}

#endif // EXT_INPUT