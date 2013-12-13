#include <Socket/server/Server.hpp>
#include <Socket/Config.hpp>

int main(int argc,char* argv[])
{
    const unsigned int max_client = 100;

    ntw::Config::max_connexion = 5;
    ntw::Config::port_server = 3987;
    ntw::Config::port_client = 3988;
    ntw::Config::default_timeout = 5.f;
    ntw::Config::broadcast = false;
    

    ntw::srv::Server server(max_client);
    server.start();

    server.wait();
    return 0;
}
