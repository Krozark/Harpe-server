#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <Socket/SocketSerialized.hpp>

enum FUNCTION_ID
{
    GET_VERSION=1,
    ANALYSE=2
};

int getVersion(ntw::SocketSerialized& sock);

int analyse(ntw::SocketSerialized& sock,int mgf_pk);  

#endif
