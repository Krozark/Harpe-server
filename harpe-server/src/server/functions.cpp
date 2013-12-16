#include <server/functions.hpp>

#include <server/defines.hpp>

int getVersion(ntw::SocketSerialized& sock)
{
    return VERSION;
};

int analyse(ntw::SocketSerialized& sock,int mgf_pk,std::string file_data)
{
    ///\todo découpage
    ///\todo save in bdd
    ///\todo send to clients
    return 42;
}
