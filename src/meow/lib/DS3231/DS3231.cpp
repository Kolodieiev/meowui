#include "DS3231.h"
#include "DS3231Util.h"

namespace meow
{
    bool DS3231::begin()
    {
        if (!_i2c.begin())
            return false;

        if (!_i2c.hasConnect(DS3231_REG_STATUS))
            return false;

        uint8_t status_reg;

        bool success = true;

        success &= _i2c.readRegister(DS3231_ADDR, DS3231_REG_STATUS, &status_reg, sizeof(status_reg));

        // Вимкунути 32kHz пін
        status_reg &= ~_BV(STS_BIT_EN32KHZ);
        success &= _i2c.writeRegister(DS3231_ADDR, DS3231_REG_STATUS, &status_reg, sizeof(status_reg));

        // Встановити 24год формат
        success &= _i2c.readRegister(DS3231_ADDR, 0x02, &status_reg, sizeof(status_reg));
        status_reg &= ~_BV(6);

        success &= _i2c.writeRegister(DS3231_ADDR, 0x02, &status_reg, sizeof(status_reg));

        if (!success)
            log_e("Помилка ініціалізації DS3231");

        return success;
    }

    bool DS3231::isDateTimeValid()
    {
        uint8_t status_reg;
        if (!_i2c.readRegister(DS3231_ADDR, DS3231_REG_STATUS, &status_reg, sizeof(status_reg)))
            return false;

        return !(status_reg & _BV(STS_BIT_OSF));
    }

    bool DS3231::isRunning()
    {
        uint8_t ctrl_reg;
        if (!_i2c.readRegister(DS3231_ADDR, DS3231_REG_CTRL, &ctrl_reg, sizeof(ctrl_reg)))
            return false;

        return !(ctrl_reg & _BV(CNTRL_BIT_EOSC));
    }

    bool DS3231::enable()
    {
        uint8_t ctrl_reg;
        if (!_i2c.readRegister(DS3231_ADDR, DS3231_REG_CTRL, &ctrl_reg, sizeof(ctrl_reg)))
            return false;

        ctrl_reg &= ~_BV(CNTRL_BIT_EOSC);
        return _i2c.writeRegister(DS3231_ADDR, DS3231_REG_CTRL, &ctrl_reg, sizeof(ctrl_reg));
    }

    bool DS3231::disable()
    {
        uint8_t ctrl_reg;
        if (!_i2c.readRegister(DS3231_ADDR, DS3231_REG_CTRL, &ctrl_reg, sizeof(ctrl_reg)))
            return false;

        ctrl_reg |= _BV(CNTRL_BIT_EOSC);
        return _i2c.writeRegister(DS3231_ADDR, DS3231_REG_CTRL, &ctrl_reg, sizeof(ctrl_reg));
    }

    bool DS3231::setDateTime(const DS3231DateTime &date_time)
    {
        uint8_t status_reg;
        if (!_i2c.readRegister(DS3231_ADDR, DS3231_REG_STATUS, &status_reg, sizeof(status_reg)))
            return false;

        status_reg &= ~_BV(STS_BIT_OSF);

        _i2c.writeRegister(DS3231_ADDR, DS3231_REG_STATUS, &status_reg, sizeof(status_reg));

        uint8_t buffer[8];
        buffer[0] = REG_TIMEDATE;
        buffer[1] = uint8ToBcd(date_time.second);
        buffer[2] = uint8ToBcd(date_time.minute);
        buffer[3] = uint8ToBcd(date_time.hour); // 24 hour mode only

        uint8_t year = date_time.year - 2000;
        uint8_t century_flag = 0;

        if (year >= 100)
        {
            year -= 100;
            century_flag = _BV(7);
        }

        // 1 = Понеділок
        uint8_t dow = dayOfWeek(date_time.year, date_time.month, date_time.day_of_month);

        buffer[4] = uint8ToBcd(dow);
        buffer[5] = uint8ToBcd(date_time.day_of_month);
        buffer[6] = uint8ToBcd(date_time.month) | century_flag;
        buffer[7] = uint8ToBcd(year);

        return _i2c.write(DS3231_ADDR, buffer, 8);
    }

    bool DS3231::connected() const
    {
        return _i2c.hasConnect(DS3231_ADDR);
    }

    DS3231DateTime DS3231::getDateTime()
    {
        DS3231DateTime dt{0, 0, 0, 0, 0, 0};

        uint8_t buffer[REG_TIMEDATE_SIZE];
        buffer[0] = REG_TIMEDATE;

        if (!_i2c.write(DS3231_ADDR, buffer, 1))
            return dt;

        if (!_i2c.read(DS3231_ADDR, buffer, REG_TIMEDATE_SIZE))
            return dt;

        uint8_t second = bcdToUint8(buffer[0] & 0x7F);
        uint8_t minute = bcdToUint8(buffer[1]);
        uint8_t hour = bcdToBin24Hour(buffer[2]);

        // buffer[3] throwing away day of week

        uint8_t day_of_month = bcdToUint8(buffer[4]);
        uint8_t month_raw = buffer[5];
        uint16_t year = bcdToUint8(buffer[6]) + 2000;

        if (month_raw & _BV(7)) // century wrap flag
            year += 100;

        dt.second = second;
        dt.minute = minute;
        dt.hour = hour;
        dt.day_of_month = day_of_month;
        dt.month = bcdToUint8(month_raw & 0x7f);
        dt.year = year;

        return dt;
    }

    uint8_t DS3231::dayOfWeek(uint16_t year, uint8_t month, uint8_t day_of_month)
    {
        for (uint8_t indexMonth = 1; indexMonth < month; ++indexMonth)
            day_of_month += *(DAYS_IN_MONTH + indexMonth - 1);

        if (month > 2 && year % 4 == 0)
            day_of_month++;

        day_of_month = day_of_month + 365 * year + (year + 3) / 4 - 1;

        return (day_of_month + 6) % 7;
    }

    DS3231DateTime DS3231::compiledToDS3231DateTime(const char *date, const char *time)
    {
        DS3231DateTime dt;

        dt.year = strToUint8(date + 9) + 2000;

        switch (date[0])
        {
        case 'J':
            if (date[1] == 'a')
                dt.month = 1;
            else if (date[2] == 'n')
                dt.month = 6;
            else
                dt.month = 7;
            break;
        case 'F':
            dt.month = 2;
            break;
        case 'A':
            dt.month = date[1] == 'p' ? 4 : 8;
            break;
        case 'M':
            dt.month = date[2] == 'r' ? 3 : 5;
            break;
        case 'S':
            dt.month = 9;
            break;
        case 'O':
            dt.month = 10;
            break;
        case 'N':
            dt.month = 11;
            break;
        case 'D':
            dt.month = 12;
            break;
        }

        dt.day_of_month = strToUint8(date + 4);
        dt.hour = strToUint8(time);
        dt.minute = strToUint8(time + 3);
        dt.second = strToUint8(time + 6);

        return dt;
    }
}