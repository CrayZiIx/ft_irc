#include "../inc/socketServer.hpp"

void FindQ(std::string command, std::string toFind, std::string &str)
{
    size_t i = 0;
    for (; i < command.size(); i++)
    {
        if (command[i] != ' ')
        {
            std::string tmp;
            for (; i < command.size() && command[i] != ' '; i++)
                tmp += command[i];
            if (tmp == toFind)
                break ;
            else
                tmp.clear();
        }
    }
    if (i < command.size())
        str = command.substr(i);
    i = 0;
    for (; i < str.size() && str[i] == ' '; i++)
        str = str.substr(i);
}

std::string SplitQuit(std::string command)
{
    std::istringstream stringSteam(command);
    std::string reason;
    std::string str;
    
    stringSteam>>str;
    FindQ(command, str, reason);
    if (reason.empty())
        return (std::string("Quit"));
    if (reason[0] != ':')
    {
        for (size_t i = 0; i < reason.size(); i++)
        {
            if (reason[i] == ' ')
            {
                reason.erase(reason.begin() + i, reason.end());
                break ;
            }
        }
        reason.insert(reason.begin(), ':');
    }
    return (reason);
}

void Server::quitCommand(std::string command, int fd)
{
    std::string reason;

    reason = SplitQuit(command);
    for (size_t i = 0; i < this->_channels.size(); i++)
    {
        if (this->_channels[i].getClient(fd))
        {
            this->_channels[i].removeClient(fd);
            if (this->_channels[i].getClientsNumber() == 0)
                this->_channels.erase(this->_channels.begin() + i);
            else
            {
                std::string rpl = ":" + this->getClientByFd(fd)->getNickName() + "!~" + this->getClientByFd(fd)->getUserName() + "@localhost QUIT " + reason + CRLF;
                this->_channels[i].sendToAll(rpl);
            }
        }
        else if (this->_channels[i].getAdmin(fd))
        {
            this->_channels[i].removeAdmin(fd);
            if (this->_channels[i].getClientsNumber() == 0)
                this->_channels.erase(this->_channels.begin() + i);
            else
            {
				std::string rpl = ":" + this->getClientByFd(fd)->getNickName() + "!~" + this->getClientByFd(fd)->getUserName() + "@localhost QUIT " + reason + CRLF;
				this->_channels[i].sendToAll(rpl);
            }
        }
    }
	std::cout<<RED<<"■"<<WHITE<<" Client ["<<fd<<"] Disconnected"<<std::endl;

}