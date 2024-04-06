#include "../inc/socketClient.hpp"
#include "../inc/socketServer.hpp"

int main(int ac, char **av)
{
    Server my_server;

    std::cout<<GREEN<<"■"<<WHITE<<" Server is starting..."<<std::endl;
    try
    {
        if (ac != 3)
            throw(std::invalid_argument("\e[1;33m■ \e[0;37musage : ./ircserv \"PORT\" \"SERVER PASSWORD\" "));
        signal(SIGINT, Server::_signalHandler);     // Catch CTRL+C signal
        signal(SIGQUIT, Server::_signalHandler);    // Catch CTRL+D signal
        my_server.serverInit(av);
    }
    catch(const std::exception &e)
    {
        my_server.closeFds();
        std::cerr<<e.what()<<std::endl;
    }
    std::cout<<RED<<"■"<<WHITE<<" Server down !"<<std::endl;
}
