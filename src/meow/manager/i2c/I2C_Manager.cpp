#pragma GCC optimize("O3")

#include "I2C_Manager.h"
#include <Wire.h>

namespace meow
{
    bool I2C_Manager::_is_inited = false;

    bool I2C_Manager::begin(I2C_MODE mode, uint8_t slave_addr, void (*receive_callback)(int), void (*request_callback)())
    {
        if (mode == I2C_MODE_MASTER)
        {
            if (!_is_inited)
                _is_inited = Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
        }
        else
        {
            if (!_is_inited)
            {
                if (!receive_callback || !request_callback)
                {
                    log_e("Відсутні обробники подій для режиму I2C_MODE_SLAVE");
                    esp_restart();
                }

                _is_inited = Wire.begin(slave_addr, PIN_I2C_SDA, PIN_I2C_SCL, 0);

                Wire.onReceive(receive_callback);
                Wire.onRequest(request_callback);
            }
        }

        if (!_is_inited)
            log_e("Помилка ініціалізіції I2C");

        return _is_inited;
    }

    void I2C_Manager::end()
    {
        Wire.end();
        Wire.flush();
        _is_inited = false;
    }

    bool I2C_Manager::hasConnect(uint8_t addr) const
    {
        if (!isInited())
            return false;

        Wire.beginTransmission(addr);

        bool success = !Wire.endTransmission();

        if (!success)
            log_e("Відсутнє з'єднання з пристроєм: %u", addr);

        return success;
    }

    bool I2C_Manager::write(uint8_t addr, const void *data_buff, size_t data_size) const
    {
        if (!isInited())
            return false;

        Wire.beginTransmission(addr);
        Wire.write(const_cast<uint8_t *>(static_cast<const uint8_t *>(data_buff)), data_size);
        return !Wire.endTransmission();
    }

    bool I2C_Manager::writeRegister(uint8_t addr, uint8_t reg, const void *data_buff, size_t data_size) const
    {
        if (!isInited())
            return false;

        Wire.beginTransmission(addr);
        Wire.write(reg);
        Wire.write(const_cast<uint8_t *>(static_cast<const uint8_t *>(data_buff)), data_size);
        return !Wire.endTransmission();
    }

    bool I2C_Manager::send(uint8_t value) const
    {
        if (!isInited())
            return false;
        return Wire.write(value) == 1;
    }

    bool I2C_Manager::send(const void *data_buff, size_t data_size) const
    {
        if (!isInited())
            return false;
        return Wire.write(const_cast<uint8_t *>(static_cast<const uint8_t *>(data_buff)), data_size) == data_size;
    }

    bool I2C_Manager::readRegister(uint8_t addr, uint8_t reg, void *out_data_buff, uint8_t data_size) const
    {
        if (!isInited())
            return false;

        Wire.beginTransmission(addr);
        Wire.write(reg);
        if (Wire.endTransmission())
            return false;

        return read(addr, out_data_buff, data_size);
    }

    bool I2C_Manager::read(uint8_t addr, void *out_data_buff, uint8_t data_size) const
    {
        if (!isInited())
            return false;

        if (Wire.requestFrom(addr, data_size) != data_size)
            return false;

        unsigned long start_time = millis();

        while (Wire.available() < data_size)
        {
            if ((millis() - start_time) > I2C_AWAIT_TIME_MS)
                return false;
        }

        uint8_t *temp_ptr = static_cast<uint8_t *>(out_data_buff);
        for (uint8_t i = 0; i < data_size; ++i)
            temp_ptr[i] = Wire.read();

        return true;
    }

    bool I2C_Manager::receive(void *out_data_buff) const
    {
        if (!isInited())
            return false;

        unsigned long start_time = millis();
        while (!Wire.available())
        {
            if ((millis() - start_time) > I2C_AWAIT_TIME_MS)
                return false;
        }

        uint8_t *temp_ptr = static_cast<uint8_t *>(out_data_buff);
        uint16_t i = 0;
        while (Wire.available())
            temp_ptr[i++] = Wire.read();

        return true;
    }

    void I2C_Manager::beginTransmission(uint8_t addr) const
    {
        if (!isInited())
            return;

        Wire.beginTransmission(addr);
    }

    bool I2C_Manager::endTransmission() const
    {
        return !Wire.endTransmission();
    }

    bool I2C_Manager::isInited() const
    {
        if (!_is_inited)
        {
            log_e("I2C не ініціалізовано");
            return false;
        }

        return true;
    }
}