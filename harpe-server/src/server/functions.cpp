#include <server/functions.hpp>

#include <server/defines.hpp>
#include <server/models.hpp>

#include <mgf/Driver.hpp>

#include <Socket/FuncWrapper.hpp>

#include <sstream>
#include <deque>
#include <memory>

#include <chrono>
#include <thread>
#include <mutex>


std::mutex peptides_mutex;
std::deque<std::shared_ptr<AnalysePeptide>> peptides;

int getVersion(ntw::SocketSerialized& sock)
{
    LOG(sock,"getVersion","");
    return VERSION;
};

int analyse(ntw::SocketSerialized& sock,int mgf_pk,std::string file_data)
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
        sock.setStatus(ERRORS::INPUT_NOT_VALID);
        return 0;
    }

    /// as peptides
    const unsigned int size = analyse.size();
    LOG(sock,"analyse",size<<" peptides to analyse");
    if(size<=0)
    {
        LOG(sock,"analyse","EMPTY_INPUT");
        sock.setStatus(ERRORS::EMPTY_INPUT);
        return 0;
    }

    ///get the analyse from bdd
    auto& bdd_analyse = AnalyseMgf::get(mgf_pk);
    std::cout<<"AnalyseMgf pk="<<mgf_pk<<" : "<<*bdd_analyse<<std::endl;

    if( not bdd_analyse)
    {
        LOG(sock,"analyse","PK_ERROR");
        sock.setStatus(ERRORS::PK_ERROR);
        return 0;
    }
    
    ///\todo save in bdd
    const std::list<mgf::Spectrum*>& spectrums = analyse.getSpectrums();
    
    peptides_mutex.lock();
    for(mgf::Spectrum* spectrum : spectrums)
    {
        peptides.emplace_back(new AnalysePeptide);
        std::shared_ptr<AnalysePeptide>& pep = peptides.back();

        pep->analyse = bdd_analyse;
        pep->name = spectrum->getHeader().getTitle();

        std::stringstream stream;
        stream<<*spectrum;
        pep->mgf_part = stream.str();

        pep->is_done = false;

        pep->save();
    }
    peptides_mutex.unlock();
    
    return size;
}

void clientWaitForWork(ntw::SocketSerialized& sock)
{
    int time = 5*60*1000; //min * secondes * millisecondes
    constexpr int delta = 500; //milisecondes

    while(time>0)
    {
        peptides_mutex.lock();
        std::cout<<"waiting"<<std::endl;
        if (peptides.size() >0)
        {
            std::shared_ptr<AnalysePeptide> pep = peptides.front();
            peptides.pop_front();
            peptides_mutex.unlock();
            
            pep->is_done = true;

            pep->save();

            sock.setStatus(ERRORS::EMPTY_DATA_SEND);
            //return std::move(*pep);
            return;
        }
        else
        {
            peptides_mutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(delta));
            time=-delta;
        }
    }
    sock.setStatus(ERRORS::TIMEOUT);
}
