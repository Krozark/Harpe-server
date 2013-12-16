///data base
#include <ORM/backends/Sqlite3.hpp>
orm::Sqlite3Bdd def("/home/maxime/Documents/git/Harpe-website/Harpe-website/dev.db");
orm::Bdd& orm::Bdd::Default = def;

#include <Socket/server/Server.hpp>
#include <Socket/Config.hpp>
#include <server/models.hpp>

int main(int argc,char* argv[])
{
    orm::Bdd::Default.connect();

    const unsigned int max_client = 100;

    ntw::Config::max_connexion = 5;
    ntw::Config::port_server = 3987;
    ntw::Config::port_client = 3988;
    ntw::Config::default_timeout = 5.f;
    ntw::Config::broadcast = false;
    

    ntw::srv::Server server(max_client);
    server.start();

    server.wait();

    orm::Bdd::Default.disconnect();

    return 0;
}
