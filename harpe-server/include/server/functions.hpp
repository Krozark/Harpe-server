#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <Socket/SocketSerialized.hpp>

class AnalysePeptide;

/**
 * \brief Define the functions ids
 */
enum FUNCTION_ID
{
    GET_VERSION=1,
    ANALYSE=2,
    CLIENT_WAIT_FOR_WORK=3,
};

/**
 * \brief Define the ERRORS values
 */
enum ERRORS {
    INPUT_NOT_VALID = 1,
    EMPTY_INPUT = 2,
    PK_ERROR = 3,
    EMPTY_DATA_SEND = 4,
    TIMEOUT = 5
};

/**
 * \brief init the deque of peptite to analyse that have been save in the DB, but not analysed
 */
int init_deque_peptide();

/**
 * \brief get the soft vesion
 */
int getVersion(ntw::SocketSerialized& sock);

/**
 * \brief add a MGF file to bee analyse
 */ 
int analyse(ntw::SocketSerialized& sock,int mgf_pk,std::string file_data);

/**
 * \brief the main client function. Waite for a job
 * and send it when avalible
 */
void clientWaitForWork(ntw::SocketSerialized& sock);

#endif
