#include "../inc/socketServer.hpp"

bool isClientInvited(Client *cli, std::string channelName, int flag)
{
    if (cli->getInviteChannel(channelName))
    {
        if (flag == 1)
            cli->removeChannelInvite(channelName);
        return (true);
    }
    return (false);
}

void Server::joinCommand(std::string command, int fd)
{
    std::vector<std::pair<std::string, std::string> > token;

    if (!joinSplit(token, command, fd))
    {
        // ERR_NEEDMOREPARAMS (461) if the channel name is empty
        this->sendError(this->getClientByFd(fd)->getClientFd(), 461, this->getClientByFd(fd)->getNickName(), " :Not enough parameters"CRLF);
        return ;
    }
    if (token.size() > 10)
    {
        //ERR_TOOMANYTARGETS (407) if more than 10 Channels
        this->sendError(this->getClientByFd(fd)->getClientFd(), 407, this->getClientByFd(fd)->getNickName(), " :Too many channels"CRLF);
        return ;
    }
    for (size_t i = 0; i < token.size(); i++)
    {
        bool flag = false;
        for (size_t j = 0; j < this->_channels.size(); j++)
        {
            if (this->_channels[i].getName() == token[i].first)
            {
                channelIsExisting(token, i, j, fd);
                flag = true;
                break;
            }
        }
        if (!flag)
            channelIsntExisting(token, i, fd);
    }
    return ;
}
void Server::channelIsExisting(std::vector<std::pair<std::string, std::string> > &token, int i, int j, int fd)
{
    if (this->_channels[j].getClientInChannel(this->getClientByFd(fd)->getNickName()))// if the client is already registered
		return;
	if (this->searchClientInChannel(this->getClientByFd(fd)->getNickName()) >= 10)//ERR_TOOMANYCHANNELS (405) // if the client is already in 10 channels
    {
        this->sendError(this->getClientByFd(fd)->getClientFd(), 405, this->getClientByFd(fd)->getNickName(), " :You have joined too many channels"CRLF);
        return;
    }
	if (!this->_channels[j].getPassword().empty() && this->_channels[j].getPassword() != token[i].second)
    {// ERR_BADCHANNELKEY (475) // if the password is incorrect
		if (!isClientInvited(this->getClientByFd(fd), token[i].first, 0))
        {
            this->sendError( this->getClientByFd(fd)->getClientFd(),475, this->getClientByFd(fd)->getNickName(), "#" + token[i].first, " :Cannot join channel (+k) - bad key"CRLF);
            return;
        }
	}
	if (this->_channels[j].getInvitOnly()){// ERR_INVITEONLYCHAN (473) // if the channel is invit only
		if (!isClientInvited(this->getClientByFd(fd), token[i].first, 1))
        {
            sendError( this->getClientByFd(fd)->getClientFd(),473, this->getClientByFd(fd)->getNickName(), "#" + token[i].first, " :Cannot join channel (+i)"CRLF);
            return;
        }
	}
	if (this->_channels[j].getLimit() && this->_channels[j].getClientsNumber() >= this->_channels[j].getLimit())// ERR_CHANNELISFULL (471) // if the channel reached the limit of number of clients
    {
        sendError(this->getClientByFd(fd)->getClientFd(),471, this->getClientByFd(fd)->getNickName(), "#" + token[i].first," :Cannot join channel (+l)"CRLF);
        return;
    }
	// add the client to the channel
	Client *cli = getClientByFd(fd);
	this->_channels[j].addClient(*cli);
	if(this->_channels[j].getTopicName().empty())
		this->sendResponse(fd, RPL_JOINMSG(this->getClientByFd(fd)->getHostname(),this->getClientByFd(fd)->getIpAddr(),token[i].first)\
            + RPL_NAMREPLY(this->getClientByFd(fd)->getNickName(),this->_channels[j].getName(),this->_channels[j].clientChannelList())\
            + RPL_ENDOFNAMES(this->getClientByFd(fd)->getNickName(),this->_channels[j].getName()));
	else
		this->sendResponse(fd, RPL_JOINMSG(this->getClientByFd(fd)->getHostname(),this->getClientByFd(fd)->getIpAddr(),token[i].first)\
            + RPL_TOPICIS(this->getClientByFd(fd)->getNickName(),this->_channels[j].getName(),this->_channels[j].getTopicName())\
            + RPL_NAMREPLY(this->getClientByFd(fd)->getNickName(),this->_channels[j].getName(),this->_channels[j].clientChannelList())\
            + RPL_ENDOFNAMES(this->getClientByFd(fd)->getNickName(),this->_channels[j].getName()));
    this->_channels[j].sendToAll(RPL_JOINMSG(this->getClientByFd(fd)->getHostname(),this->getClientByFd(fd)->getIpAddr(),token[i].first), fd);
    return ;
}

void Server::channelIsntExisting(std::vector<std::pair<std::string, std::string> > &token, int i, int fd)
{
    if (searchClientInChannel(this->getClientByFd(fd)->getNickName()) >= 10)
    {
        //ERR_TOOMANYCHANNELS (405) if the client is already in 10 Channels
        this->sendError(this->getClientByFd(fd)->getClientFd(), 405, this->getClientByFd(fd)->getNickName(), " :You have joined too many channels"CRLF);
        return ;
    }
    Channel newChannel;
    newChannel.setName(token[i].first);
    newChannel.addAdmin(*this->getClientByFd(fd));
    newChannel.setCreationTime();
    /*ICI FORCE A TOI QUI VA LIRE IL EST 5H41 DU MATIN JE FERAIS PAS UN TRUC + PROPRE x)*/
    this->sendResponse(fd, RPL_JOINMSG(this->getClientByFd(fd)->getHostname(), this->getClientByFd(fd)->getIpAddr(), newChannel.getName())\
        + RPL_NAMREPLY(this->getClientByFd(fd)->getNickName(), newChannel.getName(), newChannel.clientChannelList())\
        + RPL_ENDOFNAMES(this->getClientByFd(fd)->getNickName(), newChannel.getName()));

    return ;
}

int Server::searchClientInChannel(std::string nickName)
{
    int count = 0;
    for (size_t i = 0; i < this->_channels.size(); i++)
    {
        if (this->_channels[i].getClientInChannel(nickName))
            count++;
    }
    return (count);
}

/*Je ne sais plus ce que je fais, refacto ou rendre + clean cette fonction ???*/
int Server::joinSplit(std::vector<std::pair<std::string, std::string> > &token, std::string command, int fd)
{
    std::vector<std::string> tmp;
    std::string channelStr;
    std::string passwordStr;
    std::string buff;
    std::istringstream stringStream(command);

    while (stringStream >> command)
        tmp.push_back(command);
    if (tmp.size() < 2)
    {
        token.clear();
        return (0);
    }
    tmp.erase(tmp.begin());
    channelStr = tmp[0];
    tmp.erase(tmp.begin());
    if (!tmp.empty())
    {
        passwordStr = tmp[0];
        tmp.clear();
    }
    for (size_t i = 0; i < channelStr.size(); i++)
    {
        if (channelStr[i] == ',')
        {
            token.push_back(std::make_pair(buff, ""));
            buff.clear();
        }
        else
            buff += channelStr[i];
    }
    token.push_back(std::make_pair(buff, ""));
    if (!passwordStr.empty())
    {
        size_t j = 0;
        buff.clear();
        for (size_t i = 0; i < passwordStr.size(); i++)
        {
            if (passwordStr[i] == ',')
            {
                token[j].second = buff;
                j++;
                buff.clear();
            }
            else
                buff += passwordStr[i];
        }
        token[j].second = buff;
    }
    for (size_t i = 0; i < token.size(); i++)
    {
        if (token[i].first.empty())
            token.erase(token.begin() + i--);
    }
    for (size_t i = 0; i < token.size(); i++)
    {
        if (*(token[i].first.begin()) != '#')
        {
            sendError(this->getClientByFd(fd)->getClientFd(), 403, this->getClientByFd(fd)->getNickName(), " :No such channel"CRLF);
            token.erase(token.begin() + i--);
        }
        else
            token[i].first.erase(token[i].first.begin());
    }
    return (1);
}