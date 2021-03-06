#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <Socket/SocketSerialized.hpp>
#include <Socket/Status.hpp>

#define SERVER_NAME 1
#define WEBSITE_HOST 2
#define WEBSITE_PORT 3
#define SERVER_PORT 4
#define CLIENT_PORT 5

class AnalysePeptide;


/**
 * \brief Define the functions ids
 */
enum FUNCTION_ID
{
    GET_VERSION=1,
    ANALYSE=2,
    CLIENT_WAIT_FOR_WORK=3,
    SEND_PEPTIDE_RESULTS=4,
    SEND_CLIENT_CONFIG=5,
    RESTART=6
};

/**
 * \brief Define the ERRORS values
 */
enum ERRORS {
    INPUT_NOT_VALID = ntw::Status::user_define,
    EMPTY_INPUT,
    PK_ERROR,
    EMPTY_DATA_SEND,
    TIMEOUT
};


/**
 * \brief Define the dispatch function
 * A simple switch on the id, and call the function
 * of this id
 */
 int dispatch(int id,ntw::SocketSerialized& request);


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

/**
 * \brief recv result from client
 * \param pk the analyseMgf pk
 */
void sendPeptideResults(ntw::SocketSerialized& sock,int id,int status);

/**
 * \brief get informations from the client
 * \return true if the version are compatibles, else false
 */
bool getClientInfo(ntw::SocketSerialized& sock,int version,int ram);

/**
 * \brief todo
 * \todo
 */
int restart(ntw::SocketSerialized& sock);


/******************************************************
 ******************** REGISTER ***********************
 *****************************************************/

/**
 * \brief define some external class
 */
namespace ntw {
    namespace srv {
        class Server;
        class Client;
    }
}

/***
 * \brief Register the server to the website interface
 */
int register_to_website(int port_server,char host[],int port,const std::string& name);


/**
 * \brief Unregister the server to the website interface
 */
int unregister_to_website(int port_server,char host[],int port,const std::string& name);


bool get_register_server(const std::string& name);

/**
 * \brief call back on new client recv
 */
void register_client(ntw::srv::Server& self,ntw::srv::Client& client);

/**
 * \brief call back on delete client
 */
void unregister_client(ntw::srv::Server& self,ntw::srv::Client& client);


extern ntw::srv::Server* server;

#endif
