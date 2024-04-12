#include "../inc/socketServer.hpp"

std::string Server::mode_Append(std::string array, char oper, char mode)
{
    std::stringstream stringStream;

    stringStream.clear();
    char last = '\0';
    for (size_t i = 0; i < array.size(); i++)
    {
        if (array[i] == '+' || array[i] == '-')
            last = array[i];
    }
    if (last != oper)
        stringStream<<oper<<mode;
    else    
        stringStream<<mode;
    return (stringStream.str());
}

void Server::getCommandArgs(std::string &command,
                            std::string &channelName,
                            std::string &modeSet,
                            std::string &params)
{
    std::istringstream stringStream(command);

    stringStream >> channelName;
    stringStream >> modeSet;

    size_t found = command.find_first_not_of(channelName + modeSet + " \t\v");

    if (found != std::string::npos)
        params = command.substr(found);
}

std::vector<std::string> Server::splitParams(std::string params)
{
    if (!params.empty() && params[0] == ':')
        params.erase(params.begin());
    std::vector<std::string> tokens;
    std::string p;
    std::istringstream strinStream(params);
    while (std::getline(strinStream, p, ','))
    {
        tokens.push_back(p);
        p.clear();
    }
    return (tokens);
}

void Server::modeCommand(std::string &command, int fd)
{
    Channel *channel;
    Client *user = this->getClientByFd(fd);
    std::string channelName;
    std::string params;
    std::string modeSet;
    std::stringstream modeChain;
    std::string arguments = ":";
    char opera = '\0';
    size_t found = command.find_first_not_of("MODEmode \t\v");
    
    if (found != std::string::npos)
        command = command.substr(found);
    else
    {
        this->sendResponse(fd, ERR_NOTENOUGHPARAM(user->getNickName()));
        return ;
    }
    this->getCommandArgs(command, channelName, modeSet, params);

    std::vector<std::string> tokens = this->splitParams(params);

    if (channelName[0] != '#' || !(channel = this->getChannelByName(channelName.substr(1))))
    {
        this->sendResponse(fd, ERR_CHANNELNOTFOUND(user->getUserName(), channelName));
        return ;
    }
    else if (!channel->getClient(fd) && !channel->getAdmin(fd))
    {
        this->sendError(this->getClientByFd(fd)->getClientFd()\
                        ,442\
                        , this->getClientByFd(fd)->getNickName()\
                        , channelName\
                        , " :You're not on that channel"CRLF);
        return ;
    }
    else if (modeSet.empty())
    {
        /*envoie la reponse server en ajoutant le mode du channel !*/
        this->sendResponse(fd, RPL_CHANNELMODES(user->getNickName()\
                                                ,channel->getName()
                                                ,channel->getModes())\
                                + RPL_CREATIONTIME(user->getNickName()\
                                                ,channel->getName()\
                                                ,channel->getTimeCreation()));
        return ;
    }
    else if (!channel->getAdmin(fd))
    {
        this->sendResponse(fd, ERR_NOTOPERATOR(channel->getName()));
        return ;
    }
    else if (channel)
    {
        size_t pos = 0;

        for (size_t i = 0; i < modeSet.size(); i++)
        {
            if (modeSet[i] == '+' || modeSet[i] == '-')
                opera = modeSet[i];
            else
            {
                if (modeSet[i] == 'i')
                    modeChain << inviteOnly(channel, opera, modeChain.str());
                else if (modeSet[i] == 't')
                    modeChain << topicRestriction(channel, opera, modeChain.str());
                else if (modeSet[i] == 'k')
                    modeChain << HandlePassword(fd, tokens, channel, pos, opera, modeChain.str(), arguments);
                else if (modeSet[i] == 'o')
                    modeChain << HandleOperatorPrivilege(fd, tokens, channel, pos, opera, modeChain.str(), arguments);
                else if (modeSet[i] == 'l')
                    std::cout<<"test"<<std::endl;
                else
                    std::cout<<"test"<<std::endl;
            }
        }
    }


}


/******************************************************************/
/********************|MODE FUNCTION           |********************/
/******************************************************************/
/********************|each mode function      |********************/
/******************************************************************/

std::string Server::inviteOnly(Channel *channel, char opera, std::string array)
{
    std::string param;

    if (opera == '+' && !channel->getModeAtIndex(0))
    {
        channel->setModeAtIndex(0, true);
        channel->setInvitationOnly(1);
        param = this->mode_Append(array, opera, 'i');
    }
    else if (opera == '-' && channel->getModeAtIndex(0))
    {
        channel->setModeAtIndex(0, false);
        channel->setInvitationOnly(0);
        param = mode_Append(array, opera, 'i');
    }
    return (param);
}

std::string Server::topicRestriction(Channel *channel, char opera, std::string array)
{
    std::string param;

    if (opera == '+' && !channel->getModeAtIndex(1))
    {
        channel->setModeAtIndex(1, true);
        channel->setTopicRestriction(true);
        param = this->mode_Append(array, opera, 't');
    }
    else if (opera == '-' && channel->getModeAtIndex(1))
    {
        channel->setModeAtIndex(1, false);
        channel->setInvitationOnly(false);
        param = mode_Append(array, opera, 'i');
    }
    return (param);
}

static bool checkPassword(std::string password)
{
    if (password.empty())
        return (false);
    for (size_t i = 0; i < password.size(); i++)
    {
        if (!std::isalnum(password[i] && password[i] != '_'))
            return (false);
    }
    return (true);
}

std::string Server::HandlePassword(int fd\
                                , std::vector<std::string> tokens\
                                , Channel *channel\
                                , size_t &pos\
                                , char opera\
                                , std::string array\
                                , std::string &arguments)
{
    std::string param;
    std::string password;

    if (tokens.size())
        password = tokens[pos++];
    else
    {
        this->sendResponse(fd, ERR_NEEDMODEPARM(channel->getName()\
                                                , std::string ("(k)")));
        return (param);
    }
    if (!checkPassword(password))
    {
        this->sendResponse(fd, ERR_INVALIDMODEPARM(channel->getName()
                                                , std::string ("(k)")));
        return (param);
    }
    if (opera == '+')
    {
        channel->setModeAtIndex(2, true);
        channel->setPassword(password);
        if (!arguments.empty())
            arguments += " ";
        arguments += password;
        param = mode_Append(array, opera, 'k');
    }
    else if (opera == '-' && channel->getModeAtIndex(2))
    {
        if (password == channel->getPassword())
        {
            channel->setModeAtIndex(2, false);
            channel->setPassword("");
            param = mode_Append(array, opera, 'k');
        }
        else
            this->sendResponse(fd, ERR_KEYSET(channel->getName()));
    }

    return (param);
}

std::string Server::HandleOperatorPrivilege(int fd\
                                    , std::vector<std::string> tokens\
                                    , Channel *channel\
                                    , size_t &pos\
                                    , char opera\
                                    , std::string array\
                                    , std::string &arguments)
{
    std::string user;
    std::string param;

    if (tokens.size() > pos)
        user = tokens[pos++];
    else
    {
        this->sendResponse(fd, ERR_NEEDMODEPARM(channel->getName(), std::string("(o)")));
        return (param);
    }
    if (!channel->getClientInChannel(user))
    {
        this->sendResponse(fd, ERR_NOSUCHNICK(channel->getName(), user));
        return (param);
    }
    if (opera == '+')
    {
        channel->setModeAtIndex(3, true);
        if (channel->changeClientToAdmin(user))
        {
            param = mode_Append(array, opera, 'o');
            if (!arguments.empty())
                arguments += " ";
            arguments += user;
        }
    }
    else if (opera == '-')
    {
        channel->setModeAtIndex(3, false);
        if (channel->changeAdminToClient(user))
        {
            param = mode_Append(array, opera, 'o');
            if (!arguments.empty())
                arguments += " ";
            arguments += user;
        }
    }
    
    return (param);
}

static bool CheckLimit(std::string &limit)
{
    if (!(limit.find_first_not_of("0123456789")!= std::string::npos) && std::atoi(limit.c_str()) > 0)
        return (false);
    return (true);
}

std::string Server::HandleChannelLimits(int fd\
                                        , std::vector<std::string> tokens\
                                        , Channel *channel\
                                        , size_t &pos\
                                        , char opera\
                                        , std::string array\
                                        , std::string &arguments)
{
    std::string limit;
    std::string param;

    if (opera == '+')
    {
        if (tokens.size() > pos)
        {
            limit = tokens[pos++];
            if (!CheckLimit(limit))
                this->sendResponse(fd, ERR_INVALIDMODEPARM(channel->getName(), "(l)"));
            else
            {
                channel->setModeAtIndex(4, true);
                channel->setLimit(std::atoi(limit.c_str()));
                if (!arguments.empty())
                    arguments += " ";
                arguments += limit;
                param = mode_Append(array, opera, 'l');
            }
        }
        else
            this->sendResponse(fd, ERR_NEEDMODEPARM(channel->getName(), "(l)"));
    }
    else if (opera == '+' && channel->getModeAtIndex(4))
    {
        channel->setModeAtIndex(4, false);
        channel->setLimit(0);
        param = mode_Append(array, opera, 'l');
    }
    return (param);
}







