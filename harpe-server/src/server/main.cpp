///data base
#include <ORM/backends/Sqlite3.hpp>
orm::Sqlite3Bdd def("/home/maxime/Documents/git/Harpe-website/Harpe-website/dev.db");
orm::Bdd& orm::Bdd::Default = def;

#include <Socket/server/Server.hpp>
#include <Socket/Config.hpp>
#include <server/models.hpp>
#include <stdio.h>

#define WEBSITE_HOST 1
#define WEBSITE_PORT 2
#define SERVER_PORT 3
#define CLIENT_PORT 4


int register_to_website(char host[],int port,char name[])
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

int unregister_to_website(char host[],int port,char name[])
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

int main(int argc,char* argv[])
{
    if(argc < SERVER_PORT)
    {
        std::cout<<"Usage are: "<<argv[0]<<" <website-host> <website-port> [server-port] [client-port]"<<std::endl;
        return 1;
    }
    
    int website_port = atoi(argv[WEBSITE_PORT]);

    //// inti config
    if (argc >= SERVER_PORT+1)
        ntw::Config::port_server = atoi(argv[SERVER_PORT]);
    else
        ntw::Config::port_server = 3987;

    if (argc >= CLIENT_PORT+1)
        ntw::Config::port_client = atoi(argv[CLIENT_PORT]);
    else
        ntw::Config::port_client = 3988;

    ntw::Config::max_connexion = 5;
    ntw::Config::default_timeout = 5.f;
    ntw::Config::broadcast = false;
    const unsigned int max_client = 100;

    ///register from the website
    int return_code = register_to_website(argv[WEBSITE_HOST],website_port,"Lyre");

    if(return_code == 200)
    {
        /// inti database
        orm::Bdd::Default.connect();

        std::cout<<"[Server start] on:"
            <<"\n\tPort : "<<ntw::Config::port_server
            <<"\n\tclient port : "<<ntw::Config::port_client
            <<"\n\twebsite host : "<<argv[WEBSITE_HOST]
            <<"\n\twebiet port : "<<argv[WEBSITE_PORT]
            <<std::endl;


        ntw::srv::Server server(max_client);
        server.start();
        server.wait();

        orm::Bdd::Default.disconnect();
        ///unregister from the website
        unregister_to_website(argv[WEBSITE_HOST],website_port,"Lyre");
    }
    return return_code;
}
