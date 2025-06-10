#pragma GCC optimize("O3")
#include "DataStream.h"

namespace meow
{
    DataStream::DataStream(size_t size)
    {
        resize(size);
    }

    DataStream::~DataStream()
    {
        delete[] _buffer;
    }

    void DataStream::resize(size_t size)
    {
        _index = 0;
        _size = size;
        try
        {
            delete[] _buffer;
            _buffer = new uint8_t[size];
        }
        catch (std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }

    size_t DataStream::read(void *out, size_t len)
    {
        size_t s = space();
        if (len > s)
            len = s;

        memcpy(out, _buffer + _index, len);
        _index += len;
        return len;
    }

    size_t DataStream::drop(size_t len)
    {
        size_t s = space();
        if (len > s)
            len = s;

        _index += len;

        return len;
    }

    size_t DataStream::write(const void *data, size_t len)
    {
        size_t s = space();
        if (len > s)
            len = s;

        memcpy(_buffer + _index, data, len);
        _index += len;
        return len;
    }

    size_t DataStream::space() const
    {
        return _size - _index;
    }
}