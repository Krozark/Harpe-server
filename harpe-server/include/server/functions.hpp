#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <Socket/SocketSerialized.hpp>

class AnalysePeptide;

enum FUNCTION_ID
{
    GET_VERSION=1,
    ANALYSE=2,
    CLIENT_WAIT_FOR_WORK=3,
};

enum ERRORS {
    INPUT_NOT_VALID = 1,
    EMPTY_INPUT = 2,
    PK_ERROR = 3,
    EMPTY_DATA_SEND = 4,
    TIMEOUT = 5
};

int init_deque_peptide();

int getVersion(ntw::SocketSerialized& sock);

int analyse(ntw::SocketSerialized& sock,int mgf_pk,std::string file_data);

void clientWaitForWork(ntw::SocketSerialized& sock);

#endif
