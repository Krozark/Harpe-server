#include <server/functions.hpp>

#include <server/defines.hpp>
#include <mgf/Driver.hpp>
#include <sstream>
#include <Socket/FuncWrapper.hpp>

int getVersion(ntw::SocketSerialized& sock,int& status)
{
    LOG(sock,"getVersion","");
    return VERSION;
};

int analyse(ntw::SocketSerialized& sock,int& status,int mgf_pk,std::string file_data)
{
    LOG(sock,"analyse","");
    ///découpage
    std::istringstream stream(file_data);
    mgf::Driver driver(stream);
    mgf::Analyse analyse = driver.parse();

    if (not driver.isValid())
    {
        LOG(sock,"analyse","INPUT_NOT_VALID");
        status = ERRORS::INPUT_NOT_VALID;
        return 0;
    }

    if(analyse.size()<=0)
    {
        LOG(sock,"analyse","EMPTY_INPUT");
        status = ERRORS::EMPTY_INPUT;
        return 0;
    }

    const unsigned int size = analyse.size();
    LOG(sock,"analyse",size<<" peptides to analyse");

    ///\todo save in bdd
    for(unsigned int i=0;i<size;++i)
    {
        ///save
    }


    ///\todo send to clients
    for(unsigned int i=0;i<size;++i)
    {
        ///send
    }
    
    return size;
}
