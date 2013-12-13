#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <Socket/SocketSerialized.hpp>

enum FUNCTION_ID
{
    GET_VERSION=1
};

int getVersion(ntw::SocketSerialized& sock);

#endif
