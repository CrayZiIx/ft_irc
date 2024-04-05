#include "../inc/socketClient.hpp"
#include "../inc/socketServer.hpp"

int main()
{
    Server my_server;

    try
    {
        signal(SIGINT, Server::_signalHandler); // Catch CTRL+C signal
        signal(SIGQUIT, Server::_signalHandler);// Catch CTRL+D signal
        my_server.serverInit();
    }
    catch(const std::exception &e)
    {
        my_server.closeFds();
        std::cerr<<e.what()<<std::endl;
    }
    std::cout<<"Server is DOWN"<<std::endl;
}
