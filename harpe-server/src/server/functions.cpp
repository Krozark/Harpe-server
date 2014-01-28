#include <server/functions.hpp>

#include <server/defines.hpp>
#include <server/models.hpp>

#include <mgf/Driver.hpp>
#include <mgf/Convert.hpp>

#include <Socket/FuncWrapper.hpp>

#include <sstream>
#include <deque>
#include <memory>

#include <chrono>
#include <mutex>
#include <thread>

std::mutex peptides_mutex;
std::deque<std::shared_ptr<AnalysePeptide>> peptides;



int init_deque_peptide()
{
    std::list<orm::Cache<AnalysePeptide>::type_ptr> results;    

    AnalysePeptide::query()\
        .filter(0,"exact",AnalysePeptide::_status)\
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
    mgf::Analyse analyse = driver.parse(~mgf::Spectrum::PrepareFlags::AddSpecialsPeaks); //not add special peaks, to not have theme twice, and not store theme in the BD

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
    orm::Bdd& con = *AnalyseMgf::default_connection->clone();//a new connection
    con.connect();
    con.threadInit();

    auto& bdd_analyse = AnalyseMgf::get(mgf_pk,con);

    if( not bdd_analyse)
    {
        LOG(sock,"analyse","PK_ERROR");
        sock.setStatus(ERRORS::PK_ERROR);

        con.disconnect();
        con.threadEnd();
        delete &con;

        return 0;
    }

    ///\todo save in bdd
    const std::list<mgf::Spectrum*>& spectrums = analyse.getSpectrums();
    con.beginTransaction();
   
    peptides_mutex.lock();
    for(mgf::Spectrum* spectrum : spectrums)
    {
        peptides.emplace_back(new AnalysePeptide);
        std::shared_ptr<AnalysePeptide>& pep = peptides.back();

        pep->analyse = bdd_analyse;
        pep->name = spectrum->getHeader().getTitle();
        pep->mz = spectrum->getHeader().getMz();
        pep->intensity = spectrum->getHeader().getIntensity();
        pep->charge = spectrum->getHeader().getCharge();

        std::stringstream stream;
        stream<<*spectrum;
        pep->mgf_part = stream.str();

        pep->status = 0;

        pep->save(con);
    }
    peptides_mutex.unlock();
    
    con.endTransaction();
    con.threadEnd();
    con.disconnect();
    delete &con;   


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
            //TODO : save to DB the client -> pep link
            
            sock<<*pep;
            std::cout<<"[clientWaitForWork] <"<<sock.id()<<"> Send datas : "<<sock.size()<<" "<<sock.getStatus()<<std::endl;

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
    unsigned int size = 0;
    sock>>size;
    std::cout<<"[sendPeptideResults] <"<<sock.id()<<"> Recv solutions <"<<size<<"> for AnalyseMgf of pk <"<<id<<">"<<std::endl;

    orm::Bdd& con = *AnalysePeptide::default_connection->clone();//a new connection
    con.connect();
    con.threadInit();

    auto& pep = AnalysePeptide::get(id,con);
    pep->status = 1;

    for(unsigned int i=0;i<size;++i)
    {
        double score;
        unsigned int seq_size;
        sock>>score //score of the solution
            >>seq_size; //size of the sequence

        CalculatedPeptide result;
        result.analyse = pep;
        result.score = score;

        bool is_peak = true;
        for(unsigned int j=0;j<seq_size;++j)
        {
            if(is_peak) //peak token
            {
                double mass;
                sock>>mass;
                result.sequence+=std::to_string(mass);
            }
            else //AA token
            {
                int pk;
                sock>>pk;
                result.sequence+=std::to_string(pk);
            }
            is_peak= not is_peak; 
            if(j<seq_size-1)
                result.sequence+=",";
        }
        result.save(con);
    }

    pep->save(con);

    con.threadEnd();
    con.disconnect();
    delete &con;

    sock.clear();
    sock.setStatus(ntw::FuncWrapper::Status::ok);
    std::cout<<"[clientWaitForWork] <"<<sock.id()<<"> Send datas : "<<sock.size()<<" "<<sock.getStatus()<<std::endl;
    sock.sendCl();
}


/******************************************************************
 * ******************* REGISTER **********************************
 * ***************************************************************/

#include <Socket/server/Client.hpp>

orm::Cache<HarpeServer>::type_ptr orm_server;

int register_to_website(char host[],int port,const std::string& name)
{
    int status = 0;

    std::cout<<"[Loggin to website] "<<host<<":"<<port<<std::endl;
    ntw::Socket website_sock(ntw::Socket::Dommaine::IP,ntw::Socket::Type::TCP);
    website_sock.connect(host,port);
    std::string msg;
    msg += std::string("GET /register/?name=");
    msg += name;
    msg += "&port=";
    msg += std::to_string(ntw::Config::port_server);
    msg +=" ";
    msg += "HTTP/1.1\r\nHOST: ";
    msg += host;
    msg += "\r\n\r\n";

    char buffer[1024];
    website_sock.send(msg.c_str(),msg.size());

    int recv;
    float version = 0;
    while((recv = website_sock.receive(buffer,1024))>0)
    {
        //std::cout.write(buffer,recv);
        sscanf(buffer,"HTTP/%f %d %*s",&version,&status);
    }
    //std::cout<<std::endl<<std::flush;

    if(version > 0)
    {
        std::cout<<"Status : "<<status<<std::endl;
        switch (status)
        {
            case 200 :///ok
                {
                }break;
            case 211 : /// no name (set in code)
                {
                }break;
            case 212 :/// no port (set in code)
                {
                }break;
            case 213 :///unknow ip
                {
                }break;
            case 214 :/// no object find
                {
                }break;
            default:///?
                {
                }break;
        }
    }
    return status;
}


int unregister_to_website(char host[],int port,const std::string& name)
{
    int status = 0;

    std::cout<<"[Unloggin to website] "<<host<<":"<<port<<std::endl;
    ntw::Socket website_sock(ntw::Socket::Dommaine::IP,ntw::Socket::Type::TCP);
    website_sock.connect(host,port);
    std::string msg;
    msg += std::string("GET /unregister/?name=");
    msg += name;
    msg += "&port=";
    msg += std::to_string(ntw::Config::port_server);
    msg +=" ";
    msg += "HTTP/1.1\r\nHOST: ";
    msg += host;
    msg += "\r\n\r\n";

    char buffer[1024];
    website_sock.send(msg.c_str(),msg.size());

    int recv;
    float version = 0;
    while((recv = website_sock.receive(buffer,1024))>0)
    {
        //std::cout.write(buffer,recv);
        sscanf(buffer,"HTTP/%f %d %*s",&version,&status);
    }
    //std::cout<<std::endl<<std::flush;

    if(version > 0)
    {
        std::cout<<"Status : "<<status<<std::endl;
        switch (status)
        {
            case 200 :///ok
                {
                }break;
            case 211 : /// no name (set in code)
                {
                }break;
            case 212 :/// no port (set in code)
                {
                }break;
            case 213 :///unknow ip
                {
                }break;
            case 214 :/// no object find
                {
                }break;
            default:///?
                {
                }break;
        }
    }
    return status;
}

bool get_register_server(const std::string& name)
{
    orm_server.reset(new HarpeServer);

    HarpeServer::query()\
        .filter(true,"exact",HarpeServer::_is_active)
        .filter(name,"exact",HarpeServer::_name)
        .get(*orm_server);

    if(orm_server->getPk()<=0)
        return false;

    return true;
}

void register_client(ntw::srv::Server& self,ntw::srv::Client& client)
{
    /*Client cli;

    orm::Bdd& con = *Client::default_connection->clone();//a new connection
    con.connect();

    Client::query(con)\
        .filter(client.sock().getIp(),"exact",Client::_ip)\
        .filter(client.sock().getPort(),"exact",Client::_port)\
        .get(cli);

    cli.ip = client.sock().getIp();
    cli.port = client.sock().getPort();
    cli.server = orm_server;
    cli.is_active = true;

    cli.save(con);

    con.disconnect();
    delete &con;

    std::cout<<"Client added"<<std::endl;*/
}


void unregister_client(ntw::srv::Server& self,ntw::srv::Client& client)
{
    /*Client cli;

    orm::Bdd& con = *Client::default_connection->clone();//a new connection
    con.connect();

    Client::query(con)\
        .filter(client.sock().getIp(),"exact",Client::_ip)\
        .filter(client.sock().getPort(),"exact",Client::_port)\
        .get(cli);
    cli.is_active = false;
    cli.save(con);

    con.disconnect();
    delete &con;

    std::cout<<"Client delete"<<std::endl;*/
}
