#include "DS3231Alarm.h"
#include "DS3231Util.h"

namespace meow
{
    bool DS3231Alarm::isEnabled() const
    {
        uint8_t ctrl_reg;
        if (!_i2c.readRegister(DS3231_ADDR, DS3231_REG_CTRL, &ctrl_reg, sizeof(ctrl_reg)))
            return false;

        return ctrl_reg &= _BV(DS3231_AIFMASK);
    }

    bool DS3231Alarm::isAlarmed() const
    {
        uint8_t status_reg;

        if (!_i2c.readRegister(DS3231_ADDR, DS3231_REG_STATUS, &status_reg, sizeof(status_reg)))
            return false;

        return status_reg &= _BV(DS3231_CNTRL_BIT_A2IE);
    }

    bool DS3231Alarm::setAlarmData(const DS3231AlarmTime &alarmData) const
    {
        uint8_t buffer[3];

        uint8_t flags{0x08}; // hour minutes match

        buffer[0] = REG_ALARMTWO;

        // cppcheck-suppress badBitmaskCheck
        buffer[1] = uint8ToBcd(alarmData.minute) | ((flags & 0x01) << 7);

        // cppcheck-suppress badBitmaskCheck
        buffer[2] = uint8ToBcd(alarmData.hour) | ((flags & 0x02) << 6); // 24 hour mode only

        return _i2c.write(DS3231_ADDR, buffer, 3);
    }

    bool DS3231Alarm::enable(bool enableWithoutExternalPower) const
    {
        uint16_t data{0b00000110};
        return _i2c.writeRegister(DS3231_ADDR, DS3231_REG_CTRL, &data, sizeof(data));
    }

    bool DS3231Alarm::disable() const
    {
        uint16_t data{0b00000000};
        return _i2c.writeRegister(DS3231_ADDR, DS3231_REG_CTRL, &data, sizeof(data));
    }

    DS3231AlarmTime DS3231Alarm::getAlarmTime() const
    {
        uint8_t buffer[REG_ALARMTWO_SIZE];
        buffer[0] = REG_ALARMTWO;

        if (!_i2c.write(DS3231_ADDR, buffer, 1))
            return DS3231AlarmTime{0, 0};

        if (!_i2c.read(DS3231_ADDR, buffer, REG_ALARMTWO_SIZE))
            return DS3231AlarmTime{0, 0};

        uint8_t minute = bcdToUint8(buffer[0] & 0x7F);
        uint8_t hour = bcdToBin24Hour(buffer[1] & 0x7f);

        return DS3231AlarmTime{hour, minute};
    }

    bool DS3231Alarm::procAlarm() const
    {
        uint8_t s_reg;
        if (!_i2c.readRegister(DS3231_ADDR, DS3231_REG_STATUS, &s_reg, sizeof(s_reg)))
            return false;

        s_reg &= ~DS3231_AIFMASK; // clear the flags
        return _i2c.writeRegister(DS3231_ADDR, DS3231_REG_STATUS, &s_reg, sizeof(s_reg));
    }
}