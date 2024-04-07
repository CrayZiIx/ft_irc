#include "../inc/socketServer.hpp"

void Server::inviteCommand(std::string &command, int &fd)
{
    std::vector<std::string> stringCommand = this->splitCommand(command);

    /*ERR_NEEDMOREPARAMS(461) - not enough parameters*/
    if (stringCommand.size() < 3)
    {
        sendError(fd, 461, this->getClientByFd(fd)->getNickName(), " :Not enough parameters"CRLF);
        return ;
    }

    std::string channelName = stringCommand[2].substr(1);
    
    /*ERR_NOSUCHCHANNEL(403) - if the givem channel does not exist*/
    if (stringCommand[2][0] != '#' || !this->getChannelByName(channelName))
    {
        sendError(fd, 403, channelName, " :No such channel"CRLF);
        return ;
    }
    /*ERR_NOSUCHCHANNEL(442) - if the client is not on the channel*/
    if (!(this->getChannelByName(channelName)->getClient(fd)) && !(this->getChannelByName(channelName)->getAdmin(fd)))
    {
        sendError(fd, 442, channelName, " :You're not on that channel"CRLF);
        return ;
    }
    if (this->getChannelByName(channelName)->getClientInChannel(stringCommand[1]))
    {
        sendError(fd, 443, this->getClientByFd(fd)->getNickName(), " :is already on channel"CRLF);
        return ;
    }
    Client *cli = this->getClientByNickname(stringCommand[1]);
    if (!cli)
    {
        sendError(fd, 401, stringCommand[1], " :No such nick"CRLF);
        return ;
    }
    if (this->getChannelByName(channelName)->getInvitOnly() && !this->getChannelByName(channelName)->getAdmin(fd))
    {
        sendError(fd, 482, this->getChannelByName(channelName)->getClient(fd)->getNickName(), stringCommand[1], " :You're not channel operator"CRLF);
        return ;
    }
    if (this->getChannelByName(channelName)->getLimit() && this->getChannelByName(channelName)->getClientsNumber() >= this->getChannelByName(channelName)->getLimit())
    {
        sendError(fd, 473, this->getChannelByName(channelName)->getClient(fd)->getNickName(), channelName, " :Cannot invit to channel (+i)"CRLF);
        return ;
    }
    /*WE SEND RPL(341) if the invite was successfully sent !*/
    cli->addChannelInvite(channelName);
    std::string rep1 = ": 341 " + this->getClientByFd(fd)->getNickName()+" "+cli->getNickName()+" "+ stringCommand[2] + CRLF;
    sendResponse(fd, rep1);
    std::string rep2 = ":" + cli->getHostname() + " INVITE " + cli->getNickName() + " " + stringCommand[2] + CRLF;
    sendResponse(cli->getClientFd(), rep2);
    /*TODO FINISH*/

}