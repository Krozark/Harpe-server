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

    int return_code = 0;

    ///\todo regester to the website
    {
        std::cout<<"[Loggin to website] "<<argv[WEBSITE_HOST]<<":"<<website_port<<std::endl;
        ntw::Socket website_sock(ntw::Socket::Dommaine::IP,ntw::Socket::Type::TCP);
        website_sock.connect(argv[WEBSITE_HOST],website_port);
        std::string msg;
        msg += std::string("GET /register/?name=");
        msg += "Lyre&port=";
        msg += ntw::Config::port_server;
        msg +=" ";
        msg += "HTTP/1.1\r\nHOST: ";
        msg += argv[WEBSITE_HOST];
        msg += "\r\n\r\n";

        char buffer[1024];
        website_sock.send(msg.c_str(),msg.size());

        int recv;
        float version = 0;
        int status = 0;
        while((recv = website_sock.receive(buffer,1024))>0)
        {
            std::cout.write(buffer,recv);
            sscanf(buffer,"HTTP/%f %d %*s",&version,&status);
        }
        std::cout<<std::endl<<std::flush;

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
                    return_code=status;
                }break;
                case 212 :/// no port (set in code)
                {
                    return_code=status;
                }break;
                case 213 :///unknow ip
                {
                    return_code=status;
                }break;
                case 214 :/// no object find
                {
                    return_code=status;
                }break;
                default:///?
                {
                    return_code=status;
                }break;
            }
        }
        //website_sock.shutdown();
    }
    if(return_code == 0)
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
        //server.start();
        //server.wait();


        orm::Bdd::Default.disconnect();
        ///\todo unregister from the website
    }
    else
    {
    }

    return return_code;
}
