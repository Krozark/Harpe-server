#include <server/functions.hpp>

#include <server/defines.hpp>

int getVersion(ntw::SocketSerialized& sock)
{
    return VERSION;
};

int analyse(ntw::SocketSerialized& sock,int mgf_pk)
{
    return 42;
}
