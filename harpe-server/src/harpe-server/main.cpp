///data base
//#include <ORM/backends/Sqlite3.hpp>
//orm::Sqlite3DB def("../../Harpe-website/Harpe-website/dev.db");

#include <ORM/backends/MySql.hpp>
orm::MySqlDB def("root","toor","Harpe-website");

orm::DB& orm::DB::Default = def;

#include <Socket/server/Server.hpp>
#include <stdio.h>

#include <csignal>

#include <harpe-server/functions.hpp>
#include <harpe-server/defines.hpp>

#include <ORM/core/Tables.hpp>

#include <utils/log.hpp>


/**
 * \brief Signal hnadler to stop the server
 * proprey
 */
void stop_server_handler(int sig)
{
    utils::log::warning("Signal","Recv signal",sig,". Stoping server.\n Please wait.");
    if(server)
        server->stop();
}
/**
 *  \brief The main function who start the server
 */
int main(int argc,char* argv[])
{
    std::clog<<utils::log::colors::green<<"===\nHarpe server\nversion:"<<MAJOR_VERSION<<"."<<MINOR_VERSION<<"."<<PATCH_VERSION<<"\n===\n"<<utils::log::colors::reset<<std::endl;

    if(argc < SERVER_PORT)
    {
        utils::log::error("Usage",argv[0],"<server name> <website-host> <website-port>[server-port]");
        return 1;
    }
    
    int website_port = atoi(argv[WEBSITE_PORT]);

    //// inti config
    int port_server = 3987;

    if (argc >= SERVER_PORT+1)
        port_server = atoi(argv[SERVER_PORT]);


    ///register from the website
    int return_code = register_to_website(port_server,argv[WEBSITE_HOST],website_port,argv[SERVER_NAME]);


    if(return_code == 200)
    {
        /// inti database
        orm::DB::Default.connect();
        if(get_register_server(argv[SERVER_NAME]))
        {
            init_deque_peptide();

            utils::log::ok("Server start","on:",
                "\n\tPort :",port_server,
                //<<"\n\tclient port : "<<port_client
                "\n\twebsite host :",argv[WEBSITE_HOST],
                "\n\twebsite port :",argv[WEBSITE_PORT]
                );

            std::signal(SIGINT, stop_server_handler);

            try
            {
                server = new ntw::srv::Server(port_server,"",dispatch,100);
                server->on_new_client = register_client;
                server->on_delete_client = unregister_client;

                server->start();
                server->wait();
                delete server;
            }
            catch(ntw::SocketExeption& e)
            {
                utils::log::error("Exeption",e.what());
            }


            utils::log::error("Server is close");
            orm::DB::Default.disconnect();
        }
        else
        {
            utils::log::error("Error","whene get server info fron the DB");
        }

        ///unregister from the website
        unregister_to_website(port_server,argv[WEBSITE_HOST],website_port,argv[SERVER_NAME]);
    }
    utils::log::ok("Good bye");

    ntw::Socket::close();

    return return_code;
}
