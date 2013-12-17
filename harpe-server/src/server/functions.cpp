#include <server/functions.hpp>

#include <server/defines.hpp>
#include <server/models.hpp>

#include <mgf/Driver.hpp>

#include <Socket/FuncWrapper.hpp>

#include <sstream>
#include <list>


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

    //is a valid MGF format
    if (not driver.isValid())
    {
        LOG(sock,"analyse","INPUT_NOT_VALID");
        status = ERRORS::INPUT_NOT_VALID;
        return 0;
    }

    /// as peptides
    const unsigned int size = analyse.size();
    LOG(sock,"analyse",size<<" peptides to analyse");
    if(size<=0)
    {
        LOG(sock,"analyse","EMPTY_INPUT");
        status = ERRORS::EMPTY_INPUT;
        return 0;
    }

    ///get the analyse from bdd
    auto& bdd_analyse = AnalyseMgf::get(mgf_pk);
    std::cout<<"AnalyseMgf pk="<<mgf_pk<<" : "<<*bdd_analyse<<std::endl;

    if( not bdd_analyse)
    {
        LOG(sock,"analyse","PK_ERROR");
        status = ERRORS::PK_ERROR;
        return 0;
    }
    
    ///\todo save in bdd
    std::list<AnalysePeptide> peptides;
    const std::list<mgf::Spectrum*>& spectrums = analyse.getSpectrums();

    for(mgf::Spectrum* spectrum : spectrums)
    {
        peptides.emplace_back();
        AnalysePeptide& pep = peptides.back();

        pep.analyse = bdd_analyse;

        pep.name = spectrum->getHeader().getTitle();

        std::stringstream stream;
        stream<<*spectrum;
        pep.mgf_part = stream.str();

        pep.save();
    }

    
    ///\todo send to clients
    
    return size;
}
