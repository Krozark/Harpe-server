#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <Socket/SocketSerialized.hpp>

enum FUNCTION_ID
{
    GET_VERSION=1,
    ANALYSE=2
};

enum ERRORS {
    INPUT_NOT_VALID = 1,
    EMPTY_INPUT
};

int getVersion(ntw::SocketSerialized& sock,int& status);

int analyse(ntw::SocketSerialized& sock,int& status,int mgf_pk,std::string file_data);

#endif
