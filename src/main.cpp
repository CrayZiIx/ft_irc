#include "../inc/socketClient.hpp"
#include "../inc/socketServer.hpp"
#include "../inc/utils.h"
#include "../inc/defineMessage.hpp"

bool portValidity(std::string port)
{
    if (port.find_first_not_of("0123456789") == std::string::npos\
        && (to_int(port.c_str()) >= 1024 && to_int(port.c_str()) <= 65535))
        return (true);
    return (false);
}

int main(int ac, char **av)
{
    Server my_server;

    std::cout<<GREEN<<"■"<<WHITE<<" Server is starting..."<<std::endl;
    try
    {
        if (ac != 3)
            throw(std::invalid_argument("\e[1;33m■ \e[0;37musage : ./ircserv \"PORT\" \"SERVER PASSWORD\" "));
        else if (!portValidity(av[1]))
            throw(std::invalid_argument("\e[1;33m■ \e[0;37merror :port : must be between \e[1;31m1024 \e[0;37mand \e[1;31m65535\e[0;37m "));
        else if (std::strlen(av[2]) > 20)
            throw(std::invalid_argument("\e[1;33m■ \e[0;37merror :password : must be less than or equal to \e[1;31m20 \e[0;37mcharacters "));
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
