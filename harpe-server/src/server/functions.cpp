
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
#include <chrono>
#include <thread>

std::mutex peptides_mutex;
std::deque<std::shared_ptr<AnalysePeptide>> peptides;

int init_deque_peptide()
{
    std::list<orm::Cache<AnalysePeptide>::type_ptr> results;    

    AnalysePeptide::query()\
        .filter(false,"exact",AnalysePeptide::_is_done)\
        .orderBy("id")\
        .get(results);

    std::cout<<"[init_deque_peptide] "<<results.size()<<" peptides to calc"<<std::endl;
    for(auto& i : results)
    {
        peptides.emplace_back(std::move(i));
    }
}

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
    int time = 1*60*1000; //min * secondes * millisecondes
    constexpr int delta = 500; //milisecondes

    while(time>0)
    {
        peptides_mutex.lock();
        if (peptides.size() >0)
        {
            std::shared_ptr<AnalysePeptide> pep = peptides.front();
            peptides.pop_front();
            peptides_mutex.unlock();
            std::string part = pep->mgf_part;
            //pep->is_done = true;

            //pep->save();
            
            sock<<*pep;
            std::cout<<"[clientWaitForWork] Send datas : "<<sock.size()<<" "<<sock.getStatus()<<std::endl;

            return;
        }
        else
        {
            peptides_mutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(delta));
            time-=delta;
        }
    }
    sock.setStatus(ERRORS::TIMEOUT);
}

void sendPeptideResults(ntw::SocketSerialized& sock,int id)
{
    std::cout<<"Recv solutions for AnalyseMgf of pk "<<id<<std::endl;
    unsigned int size = 0;
    sock>>size;
    std::cout<<"size: "<<size<<std::endl;

    for(unsigned int i=0;i<size;++i)
    {
        double score;
        unsigned int seq_size;
        sock>>score //score of the solution
            >>seq_size; //size of the sequence

        bool is_peak = true;
        std::cout<<"#"<<i<<"["<<score<<"] ";
        for(unsigned int j=0;j<seq_size;++j)
        {
            if(is_peak) //peak token
            {
                double mass;
                sock>>mass;
                std::cout<<"peak("<<mass<<"),";
            }
            else //AA token
            {
                int pk;
                sock>>pk;
                std::cout<<"AA("<<pk<<"),";
            }
            is_peak= not is_peak; 
        }
        std::cout<<std::endl;
    }

    sock.clear();
    sock.setStatus(ntw::FuncWrapper::Status::ok);
    std::cout<<"[clientWaitForWork] Send datas : "<<sock.size()<<" "<<sock.getStatus()<<std::endl;
    sock.sendCl();
}


