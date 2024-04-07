#include "../inc/socketServer.hpp"


/******************************************************************/
/********************|METHODE                 |********************/
/******************************************************************/
/********************|Authentification funct. |********************/
/******************************************************************/

bool Server::notRegistered(int fd)
{
    if (!this->getClientByFd(fd) || this->getClientByFd(fd)->getNickName().empty()\
    || this->getClientByFd(fd)->getUserName().empty() || this->getClientByFd(fd)->getNickName() == "*"\
    || !this->getClientByFd(fd)->getLogedIn())
        return (false);
    return (true);
}

bool Server::isValidNickname(std::string& nickname)
{
    if (!nickname.empty() && (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':'))
        return (false);
    for (size_t i = 1; i < nickname.size(); i++)
    {
        if (!std::isalnum(nickname[i]) && nickname[i] != '_')
            return (false);
    }
    return (true);
}

bool Server::nickNameInUser(std::string& nickname)
{
    for (size_t i = 0; i < this->_clients.size(); i++)
    {
        if (this->getClientByNickname(nickname))
            return (true);
    }
    return (false);
}


void Server::setNickname(std::string command, int fd)
{
    std::string inuser;
    command = command.substr(4);
    size_t pos = command.find_first_not_of("\t\v ");

    if (pos < command.size())
    {
        command = command.substr(pos);
        if (command[0] == ':')
            command.erase(command.begin());
    }
    Client *cli = this->getClientByFd(fd);
    if (pos == std::string::npos || command.empty())
    {
        sendResponse(fd, ERR_NOTENOUGHPARAM(std::string("*")));
        return ;
    }
    if (nickNameInUser(command) && cli->getNickName() != command)
    {
        inuser = "*";
        if (cli->getNickName().empty())
            cli->setNickname(inuser);
        sendResponse(fd, ERR_NICKINUSE(std::string(command)));
        return ;
    }
    if (!isValidNickname(command))
    {
        sendResponse(fd, ERR_NONICKNAME(std::string(command)));
        return ;
    }
    if (cli && cli->getRegistered())
    {
        std::string oldNickName = cli->getNickName();
        cli->setNickname(command);
        if (!oldNickName.empty() && oldNickName != command)
        {
            if (oldNickName == "*" && !cli->getUserName().empty())
            {
                cli->setLogedIn(true);
                sendResponse(fd, RPL_CONNECTED(cli->getNickName()));
                sendResponse(fd, RPL_NICKCHANGE(cli->getNickName(), command));
            }
            else
                sendResponse(fd, RPL_NICKCHANGE(oldNickName, command));
            return ;
        }
    }
    else if (cli && !cli->getRegistered())
        sendResponse(fd, ERR_NOTREGISTERED(command));
    if (cli && cli->getRegistered() && !cli->getUserName().empty()\
        && !cli->getNickName().empty() && cli->getNickName() != "*"\
        && !cli->getLogedIn())
    {
        cli->setLogedIn(true);
        sendResponse(fd, RPL_CONNECTED(cli->getNickName()));
    }
    return ;
}

void Server::setUsername(std::string &command, int fd)
{
    std::vector<std::string> commandSplited = splitCommand(command);

    Client *cli = this->getClientByFd(fd);
    if (cli && commandSplited.size() < 5)
    {
        sendResponse(fd, ERR_NOTENOUGHPARAM(cli->getNickName()));
        return ;
    }
    if (!cli || !cli->getRegistered())
    {
        sendResponse(fd, ERR_NOTREGISTERED(std::string("*")));
        return ;
    }
    else
        cli->setUsername(commandSplited[1]);
    if (cli && cli->getRegistered() && !cli->getUserName().empty() && !cli->getNickName().empty()\
        && cli->getNickName() != "*" && !cli->getLogedIn())
    {
        cli->setLogedIn(true);
        sendResponse(fd, RPL_CONNECTED(cli->getNickName()));
    }
}

void Server::authentifcateClient(int fd, std::string command)
{
    Client *cli = this->getClientByFd(fd);
    command = command.substr(4);
    size_t pos = command.find_first_not_of("\t\v ");

    if (pos < command.size())
    {
        command = command.substr(pos);
        if (command[0] == ':')
            command.erase(command.begin());
    }
    if (pos == std::string::npos || command.empty())
        sendResponse(fd, ERR_NOTENOUGHPARAM(std::string("*")));
    else if (!cli->getRegistered())
    {
        std::string pass = command;
        std::cout<<pass<<std::endl;
        if (pass == this->_servPassword)
            cli->setRegistered(true);
        else
            sendResponse(fd, ERR_INCORPASS(std::string("*")));
    }
    else
        sendResponse(fd, ERR_ALREADYREGISTERED(this->getClientByFd(fd)->getNickName()));
}