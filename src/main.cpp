#include "../inc/socketClient.hpp"
#include "../inc/socketServer.hpp"

int main(int ac, char **av)
{
    Server my_server;

    try
    {
        if (ac != 2)
            throw(std::invalid_argument("usage : ./ircserv \"PORT\" "));
        signal(SIGINT, Server::_signalHandler); // Catch CTRL+C signal
        signal(SIGQUIT, Server::_signalHandler);// Catch CTRL+D signal
        my_server.serverInit(av[1]);
    }
    catch(const std::exception &e)
    {
        my_server.closeFds();
        std::cerr<<e.what()<<std::endl;
    }
    std::cout<<"Server is DOWN"<<std::endl;
}
