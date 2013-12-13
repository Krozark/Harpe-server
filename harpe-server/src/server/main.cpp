#include <Socket/server/Server.hpp>


int main(int argc,char* argv[])
{
    const unsigned int max_client = 100;
    ntw::srv::Server server(max_client);
    server.start();

    server.wait();
    return 0;
}
