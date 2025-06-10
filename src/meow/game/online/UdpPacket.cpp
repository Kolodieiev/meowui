#pragma GCC optimize("O3")
#include "UdpPacket.h"
#include <cstring>

namespace meow
{
    UdpPacket::UdpPacket(AsyncUDPPacket &packet) : DataStream(packet.length() < 2 ? 2 : packet.length() + 1)
    {
        try
        {
            _buffer[0] = TYPE_DATA;
            _buffer[_size - 1] = '\0';

            --_size;
            _index = 1;

            memcpy(_buffer, packet.data(), _size);

            _remote_ip = packet.remoteIP();
            _port = packet.remotePort();
        }
        catch (std::exception &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }

    UdpPacket::UdpPacket(size_t data_len) : DataStream(data_len + 2)
    {
        try
        {
            _buffer[0] = TYPE_DATA;
            _buffer[_size - 1] = '\0';

            --_size; // Hide \0
            _index = 1;
        }
        catch (std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }

    const char *UdpPacket::getData(uint16_t data_pos) const
    {
        if (data_pos >= _size)
        {
            log_e("Некоректний запит. Розмір: %zu Запит: %d ", _size - 1, data_pos);
            data_pos = 0;
        }

        return (const char *)&_buffer[1 + data_pos];
    }

    void UdpPacket::printToLog(bool char_like) const
    {
        log_i("PacketType: %d", _buffer[0]);
        log_i("Data size: %zu", _size);
        log_i("Data:");

        if (char_like)
        {
            for (size_t i = 1; i < _size; ++i)
                log_i("%c", _buffer[i]);
        }
        else
        {
            for (size_t i = 1; i < _size; ++i)
                log_i("%#04x", _buffer[i]);
        }
    }

    bool UdpPacket::isDataEquals(const void *data, size_t start_pos, size_t data_len) const
    {
        ++start_pos;

        if (!data || start_pos >= _size)
            return false;

        if (data_len == 0)
            data_len = _size - start_pos;
        else if (start_pos + data_len > _size)
            return false;

        return std::memcmp(&_buffer[start_pos], data, data_len) == 0;
    }

    size_t UdpPacket::extractBytes(void *out, size_t start_pos, size_t len) const
    {
        ++start_pos;
        if (start_pos >= _size)
            return 0;

        size_t available = _size - start_pos;

        if (len > available)
            len = available;

        memcpy(out, _buffer + start_pos, len);

        return len;
    }
}