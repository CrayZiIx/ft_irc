#include "../inc/socketClient.hpp"

Client::Client()
{
    return ;
}

Client::~Client()
{
    return ;
}

int Client::getFd()
{
    return (this->_fd);
}
void Client::setFd(int fd)
{
    this->_fd = fd;
    return ;
}

void Client::setIpAddr(std::string ipAddr)
{
    this->_ipAddr = ipAddr;
    return ;
}