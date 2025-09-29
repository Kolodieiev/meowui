#pragma GCC optimize("O3")

#include "GT911.h"
#include <Wire.h>
#include "meowui_setup/input_setup.h"
#include "meowui_setup/i2c_setup.h"

namespace meow
{
    // TODO переписати на i2c_manager. Покращити наслідування.
    bool GT911::begin(uint8_t pin_int, uint8_t pin_rst, uint16_t width, uint16_t height, uint8_t addr)
    {
        _pin_int = pin_int;
        _pin_rst = pin_rst;
        _width = width;
        _height = height;
        _addr = addr;

        bool result = Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

        if (result)
        {
            Wire.beginTransmission(addr);
            result = !Wire.endTransmission();

            if (result)
            {
                init();
                return true;
            }
        }

        log_e("Помилка ініціалізації GT911");
        return false;
    }

    void GT911::stop()
    {
        Wire.end();
        Wire.flush();
    }

    void GT911::setRotation(uint8_t rotation)
    {
        if (rotation > ROTATION_270)
            rotation = ROTATION_270;

        _rotation = rotation;
    }

    void GT911::__update()
    {
        if (_is_locked)
        {
            if (millis() - _lock_time < _lock_duration)
                return;

            _is_locked = false;
            writeByteData(GT911_POINT_INFO, 0);
            return;
        }

        uint8_t pointInfo = readByteData(GT911_POINT_INFO);
        uint8_t bufferStatus = pointInfo >> 7 & 1;

        if (bufferStatus == 1 && pointInfo & 0xF > 0)
        {
            uint8_t data[7];
            readBlockData(data, GT911_POINT_1, 7);
            readPoint(data);
            _is_holded = true;
        }
        else
            _is_holded = false;

        writeByteData(GT911_POINT_INFO, 0);

        if (!_is_holded)
        {
            if (_has_touch)
            {
                _has_touch = false;
                _is_released = true;

                int x_diff = _x_s - _x_e;
                int y_diff = _y_s - _y_e;

                if (x_diff < 0)
                    x_diff = (~x_diff) + 1;

                if (y_diff < 0)
                    y_diff = (~y_diff) + 1;

                if (x_diff > 50 || y_diff > 50)
                {
                    _is_swiped = true;

                    if (x_diff >= y_diff) // Свайп по х
                    {
                        if (_x_s > _x_e)
                            _swipe = SWIPE_R;
                        else
                            _swipe = SWIPE_L;
                    }
                    else // Свайп по у
                    {
                        if (_y_s > _y_e)
                            _swipe = SWIPE_D;
                        else
                            _swipe = SWIPE_U;
                    }
                }
            }

            return;
        }

        if (_has_touch)
        {
            if (!_is_pressed)
                if (millis() - _hold_duration > PRESS_DURATION)
                {
                    _has_touch = false;
                    _is_pressed = true;
                }
        }
        else
        {
            _has_touch = true;
            _is_released = false;
            _hold_duration = millis();
        }
    }

    void GT911::init()
    {
        pinMode(_pin_int, OUTPUT);
        pinMode(_pin_rst, OUTPUT);
        digitalWrite(_pin_int, 0);
        digitalWrite(_pin_rst, 0);
        delay(10);
        digitalWrite(_pin_int, _addr == GT911_ADDR2);
        delay(1);
        digitalWrite(_pin_rst, 1);
        delay(5);
        digitalWrite(_pin_int, 0);
        delay(50);
        pinMode(_pin_int, INPUT);
        delay(50);
        readBlockData(_conf_buf, GT911_CONFIG_START, GT911_CONFIG_SIZE);
        setResolution(_width, _height);
    }

    void GT911::readPoint(uint8_t *data)
    {
        uint16_t temp;
        uint16_t x = data[1] + (data[2] << 8);
        uint16_t y = data[3] + (data[4] << 8);

        switch (_rotation)
        {
        case ROTATION_90:
            temp = x;
            x = _height - y;
            y = temp;
            break;
        case ROTATION_180:
            x = _width - x;
            y = _height - y;
            break;
        case ROTATION_270:
            temp = x;
            x = y;
            y = _width - temp;
            break;
        default:
            break;
        }

        if (_is_holded)
        {
            _x_e = x;
            _y_e = y;
        }
        else
        {
            _x_s = x;
            _y_s = y;
            _x_e = x;
            _y_e = y;
        }
    }

    void GT911::setResolution(uint16_t width, uint16_t height)
    {
        _conf_buf[GT911_X_OUTPUT_MAX_LOW - GT911_CONFIG_START] = lowByte(width);
        _conf_buf[GT911_X_OUTPUT_MAX_HIGH - GT911_CONFIG_START] = highByte(width);
        _conf_buf[GT911_Y_OUTPUT_MAX_LOW - GT911_CONFIG_START] = lowByte(height);
        _conf_buf[GT911_Y_OUTPUT_MAX_HIGH - GT911_CONFIG_START] = highByte(height);
        reflashConfig();
    }

    void GT911::calculateChecksum()
    {
        uint8_t checksum{0};
        for (uint16_t i = 0; i < GT911_CONFIG_SIZE; i++)
            checksum += _conf_buf[i];

        checksum = (~checksum) + 1;
        _conf_buf[GT911_CONFIG_CHKSUM - GT911_CONFIG_START] = checksum;
    }

    void GT911::reflashConfig()
    {
        calculateChecksum();
        writeByteData(GT911_CONFIG_CHKSUM, _conf_buf[GT911_CONFIG_CHKSUM - GT911_CONFIG_START]);
        writeByteData(GT911_CONFIG_FRESH, 1);
    }

    void GT911::writeByteData(uint16_t reg, uint8_t val)
    {
        Wire.beginTransmission(_addr);
        Wire.write(highByte(reg));
        Wire.write(lowByte(reg));
        Wire.write(val);
        Wire.endTransmission();
    }

    uint8_t GT911::readByteData(uint16_t reg)
    {
        uint8_t x;
        Wire.beginTransmission(_addr);
        Wire.write(highByte(reg));
        Wire.write(lowByte(reg));
        Wire.endTransmission();
        Wire.requestFrom(_addr, (uint8_t)1);
        x = Wire.read();
        return x;
    }

    void GT911::writeBlockData(uint16_t reg, uint8_t *val, uint8_t size)
    {
        Wire.beginTransmission(_addr);

        Wire.write(highByte(reg));
        Wire.write(lowByte(reg));
        for (uint8_t i = 0; i < size; i++)
            Wire.write(val[i]);

        Wire.endTransmission();
    }

    void GT911::readBlockData(uint8_t *buf, uint16_t reg, uint8_t size)
    {
        Wire.beginTransmission(_addr);

        Wire.write(highByte(reg));
        Wire.write(lowByte(reg));

        Wire.endTransmission();

        Wire.requestFrom(_addr, size);
        for (uint8_t i = 0; i < size; i++)
            buf[i] = Wire.read();
    }
}