#include "../inc/socketServer.hpp"

void FindPM(std::string command, std::string toFind, std::string &str)
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
    for (; i < str.size() && str[i] == ' '; i++);
    str = str.substr(i);
}

std::string SplitCmdPM(std::string &command, std::vector<std::string> &tmp)
{
    std::stringstream stringStream(command);
    std::string str;
    std::string msg;
    int count = 2;
    while (stringStream >> str && count--)
        tmp.push_back(str);
    if (tmp.size() != 2)
        return (std::string(""));
    FindPM(command, tmp[1], msg);
    return (msg);
}

std::string SplitCmdPrivmsg(std::string command, std::vector<std::string> &tmp)
{
    std::string str = SplitCmdPM(command, tmp);

    if (tmp.size() != 2)
    {
        tmp.clear();
        return (std::string(""));
    }
    tmp.erase(tmp.begin());
    std::string str1 = tmp[0];
    std::string str2;
    tmp.clear();
    for (size_t i = 0; i < str1.size(); i++)
    {
        if (str1[i] == ',')
        {
            tmp.push_back(str2);
            str2.clear();
        }
        else
            str2 += str1[i];
    }
    tmp.push_back(str2);
    for (size_t i = 0; i < tmp.size(); i++)
    {
        if (tmp[i].empty())
            tmp.erase(tmp.begin() + i--);
    }
    if (str[0] == ':')
        str.erase(str.begin());
    else // here Shrink to the first space
    {
        for (size_t i = 0; i < str.size(); i++)
        {
            if (str[i] == ' ')
            {
                str = str.substr(0, i);
                break ;
            }
        }
    }
    return (str);
}

void Server::checkForChannelsClients(std::vector<std::string>  &tmp, int fd)
{
    for (size_t i = 0; i < tmp.size(); i++)
    {
        if (tmp[i][0] == '#')
        {
            tmp[i].erase(tmp[i].begin());
            if (!this->getChannelByName(tmp[i]))
            {
                sendError(this->getClientByFd(fd)->getClientFd(), 401, "#" + tmp[i], " :Cannot send to channel"CRLF);
                tmp.erase(tmp.begin() + i);
                i--;
            }
            else
                tmp[i] = "#" + tmp[i];
        }
        else
        {
            if (!this->getClientByNickname(tmp[i]))
                sendError(this->getClientByFd(fd)->getClientFd(), 401, "#" + tmp[i], " :Cannot send to channel"CRLF);
        }
    }
    return ;
}

void Server::privmsgCommand(std::string command, int fd)
{
    std::vector<std::string> tmp;
    std::string message = SplitCmdPrivmsg(command, tmp);

    if (!tmp.size())
    {
        sendError(this->getClientByFd(fd)->getClientFd(), 411, this->getClientByFd(fd)->getNickName(), " :No recipient give (PRIVMSG)"CRLF);
        return ;
    }
    if (message.empty())
    {
        sendError(this->getClientByFd(fd)->getClientFd(), 412, this->getClientByFd(fd)->getNickName(), " :No text to send"CRLF);
        return ;
    }
    if (tmp.size() > 10)
    {
        sendError(this->getClientByFd(fd)->getClientFd(), 407, this->getClientByFd(fd)->getNickName(), " :Too many recipients"CRLF);
        return ;
    }
    checkForChannelsClients(tmp, fd);
    for (size_t i = 0; i < tmp.size(); i++)
    {
        if (tmp[i][0] == '#')
        {
            tmp[i].erase(tmp[i].begin());
            std::string resp = ":" + this->getClientByFd(fd)->getNickName() + "!~" + this->getClientByFd(fd)->getUserName() + "@localhost PRIVMSG #" + tmp[i] + " :" + message + CRLF;
            this->getChannelByName(tmp[i])->sendToAll(resp, fd);
        }
        else
        {
            std::string resp = ":" + this->getClientByFd(fd)->getNickName() + "!~" + this->getClientByFd(fd)->getUserName() + "@localhost PRIVMSG " + tmp[i] + " :" + message + CRLF;
            sendResponse(this->getClientByNickname(tmp[i])->getClientFd(), resp);
        }
    }
}