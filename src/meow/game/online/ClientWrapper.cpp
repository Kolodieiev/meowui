#pragma GCC optimize("O3")
#include "ClientWrapper.h"

namespace meow
{
    void ClientWrapper::setName(const char *name)
    {
        if (name)
            _name = name;
        else
            log_e("Ім'я клієнта не може бути null");
    }

    bool ClientWrapper::hasName(const char *name) const
    {
        if (!name)
            return false;

        return std::strcmp(_name.c_str(), name) == 0;
    }

    bool ClientWrapper::is(ClientWrapper *cl_wrap) const
    {
        if (!cl_wrap)
            return false;

        return _remote_ip == cl_wrap->_remote_ip;
    }
}