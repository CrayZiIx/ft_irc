#include "../inc/socketServer.hpp"

static void FindKick(std::string command, std::string toFind, std::string &str)
{
    std::string tmp;
    size_t i = 0;

    for (; i < command.size(); i++)
    {
        if (command[i] != ' ')
        {
            for (; i < command.size() && command[i] != ' '; i++)
                tmp += command[i];
            if (tmp == toFind)
                break;
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

static std::string extractKickReason(std::string &command, std::vector<std::string> &tmp)
{
    std::stringstream stringStream(command);
    std::string str;
    std::string reason;
    int count(3);

    while (stringStream >> str && count--)
        tmp.push_back(str);
    if (tmp.size() != 3)
        return (std::string(""));
    FindKick(command, tmp[2], reason);
    return (reason);
}

std::string Server::getKickReason(int fd, std::string command, std::vector<std::string> &tmp, std::string &user)
{
    std::string reason = extractKickReason(command, tmp);
    std::string channelList;
    std::string currentChannel;

    if (tmp.size() < 3)
        return ("");
    tmp.erase(tmp.begin());
    currentChannel = tmp[0];
    user = tmp[1];
    tmp.clear();
    for (size_t i = 0; i < channelList.size(); i++)
    {
        if (channelList[i] == ',')
        {
            tmp.push_back(currentChannel);
            currentChannel.clear();
        }
        else
            currentChannel += channelList[i];
    }
    tmp.push_back(currentChannel);
    for (size_t i = 0; i < tmp.size(); i++)
    {
        if (tmp[i].empty())
            tmp.erase(tmp.begin() + i--);
    }
    if (reason[0] == ':')
        reason.erase(reason.begin());
    else
    {
        for (size_t i = 0; i < reason.size(); i++)
        {
            if (reason[i] == ' ')
            {
                reason = reason.substr(0, i);
                break;
            }
        }
    }
    for (size_t i = 0; i < tmp.size(); i++)
    {
        if (*(tmp[i].begin()) == '#')
            tmp[i].erase(tmp[i].begin());
        else
        {
            this->sendError(this->getClientByFd(fd)->getClientFd(), 403, this->getClientByFd(fd)->getNickName(),  " :No such channel"CRLF);
            tmp.erase(tmp.begin() + i--);
        }
    }

    return (reason);
}

void Server::kickCommand(const std::string &command, int fd)
{
    std::vector<std::string> tmp;
    std::string reason;
    std::string user;
    std::stringstream stringStream;
    Channel *channel;


    reason = getKickReason(fd, command, tmp, user);
    if (user.empty())
    {
        this->sendError(this->getClientByFd(fd)->getClientFd(), 461, this->getClientByFd(fd)->getNickName(), " :Not enough parameters"CRLF);
        return ;
    }
    for (size_t i = 0; i < tmp.size(); i++)
    {
        if (this->getChannelByName(tmp[i]))
        {
            channel = this->getChannelByName(tmp[i]);
            if (!channel->getClient(fd) && !channel->getAdmin(fd))
            {
                this->sendError(this->getClientByFd(fd)->getClientFd(), 442, this->getClientByFd(fd)->getNickName(), "#" + tmp[i], " :You're not on that channel"CRLF);
                continue ;
            }
            if (channel->getAdmin(fd))
            {
                if (channel->getClientInChannel(user))
                {
                    stringStream<<":"<<this->getClientByFd(fd)->getNickName()<<"!~"<<this->getClientByFd(fd)->getUserName()<<"@localhost KICK #"<<tmp[i]<<" "<<user;
                    if (!reason.empty())
                        stringStream<<" :"<<reason<<CRLF;
                    else
                        stringStream<<CRLF;
                    channel->sendToAll(stringStream.str());
                    if (channel->getAdmin(channel->getClientInChannel(user)->getClientFd()))
                        channel->removeAdmin(channel->getClientInChannel(user)->getClientFd());
                    else
                        channel->removeClient(channel->getClientInChannel(user)->getClientFd());
                    if (channel->getClientsNumber() == 0)
                        this->_channels.erase(this->_channels.begin() + i );
                }
                else
                {
                    this->sendError(this->getClientByFd(fd)->getClientFd(), 441, this->getClientByFd(fd)->getNickName(), "#" + tmp[i], " :They aren't on that channel"CRLF);
                    continue ;
                }
            }
            else
            {
                this->sendError(this->getClientByFd(fd)->getClientFd(), 482, this->getClientByFd(fd)->getNickName(), "#" + tmp[i], " :You're not channel operator"CRLF);
                continue ;
            }
        }
        else
            this->sendError(this->getClientByFd(fd)->getClientFd(), 403, this->getClientByFd(fd)->getNickName(), "#" + tmp[i], " :No such channel"CRLF);
    }
    return ;
}
