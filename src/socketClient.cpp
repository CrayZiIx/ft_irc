#include "../inc/socketClient.hpp"

/******************************************************************/
/********************|COPLIEN FUNCTION        |********************/
/******************************************************************/

Client::Client()
{
	this->_fd = -1;
    this->_nickName = "";
	this->_userName = "";
	this->_operator= false;
	this->_registered = false;
	this->_buffer = "";
	this->_ipAddr = "";
	this->_logedin = false;
    return ;
}


Client::Client(std::string nickname, std::string username, int fd)
{
    this->_fd = fd;
    this->_userName = username;
    this->_nickName = nickname;

    return ;
}

Client::Client(const Client &other)
{
    *this = other;
    return ;
}

Client &Client::operator=(const Client &other)
{
    this->_fd = other._fd;
    this->_userName = other._userName;
    this->_nickName = other._nickName;
    this->_ipAddr = other._ipAddr;
    this->_logedin = other._logedin;
    this->_operator = other._operator;
    this->_registered = other._registered;
    this->_buffer = other._buffer;
    return (*this);
}

Client::~Client()
{
    return ;
}

/******************************************************************/
/********************|METHODE                 |********************/
/******************************************************************/
/********************|Getters                 |********************/
/******************************************************************/

int Client::getClientFd()
{
    return (this->_fd);
}

std::string Client::getUserName()
{
    return (this->_userName);
}

std::string Client::getBuffer()
{
    return (this->_buffer);
}

std::string Client::getNickName()
{
    return (this->_nickName);
}


bool Client::getRegistered()
{
    return (this->_registered);
}

bool Client::getInviteChannel(std::string &channelName)
{
    for (size_t i = 0; i < this->channelInvite.size(); i++)
    {
        if (this->channelInvite[i] == channelName)
            return (true);
    }
    return (false);
}

bool Client::getLogedIn()
{
    return (this->_logedin);
}

std::string Client::getIpAddr()
{
    return (this->_ipAddr);
}

std::string Client::getHostname()
{
    return (this->getNickName() + "!" + this->getUserName());
}


void Client::setClientFd(int fd)
{
    this->_fd = fd;
    return ;
}

void Client::setClientIpAddr(std::string ipAddr)
{
    this->_ipAddr = ipAddr;
    return ;
}

void Client::setUsername(std::string &userName)
{
    this->_userName = userName;
    return ;
}

void Client::setNickname(std::string &nickName)
{
    this->_nickName = nickName;
    return ;
}

void Client::setLogedIn(bool value)
{
    this->_logedin = value;
    return ;
}

void Client::setBuffer(std::string recived)
{
    this->_buffer += recived;
    return ;
}

void Client::setRegistered(bool value)
{
    this->_registered = value;
    return ;
}

void Client::clearBuffer()
{
    this->_buffer.clear();
    return ;
}

void Client::addChannelInvite(std::string &channelName)
{
    channelInvite.push_back(channelName);
    return ;
}

void Client::removeChannelInvite(std::string &channelName)
{
    for (size_t i = 0; i < this->channelInvite.size(); i++)
    {
        if (channelName == this->channelInvite[i])
        {
            this->channelInvite.erase(this->channelInvite.begin() + i);
            return ;
        }
    }
    return ;
}