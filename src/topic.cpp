#include "../inc/socketServer.hpp"

static std::string timeTopic()
{
    std::time_t current = std::time(NULL);
    std::stringstream stringStream;

    stringStream << current;
    return (stringStream.str());
}

// static std::string getTopic(std::string &input)
// {
//     size_t pos = input.find(":");

//     if (pos == std::string::npos)
//         return ("");
//     return (input.substr(pos));
// }

static size_t getPos(std::string &command)
{
    for (size_t i = 0; i < command.size(); i++)
    {
        if (command[i] == ':'&& command[i - 1] == 32)
            return (i);
    }
    return (-1);
}

void Server::topicCommand(std::string &command, int fd)
{
    if (command == "TOPIC :")
    {
        this->sendError(fd, 461, this->getClientByFd(fd)->getNickName(), " :Not enough parameters"CRLF);
        return ;
    }
    std::vector<std::string> arrayCommand = splitCommand(command);

    if (arrayCommand.size() == 1)
    {
        this->sendError(fd, 461, this->getClientByFd(fd)->getNickName(), " :Not enough parameters"CRLF);
        return ;
    }
    std::string channelName = arrayCommand[1].substr(1);

    if (!this->getChannelByName(channelName))
    {
        this->sendError(fd, 403, "#" + channelName, " :No such channel"CRLF);
        return ;
    }
    if (!this->getChannelByName(channelName)->getClient(fd) && !(this->getChannelByName(channelName)->getAdmin(fd)))
    {
        this->sendError(fd, 442, "#" + channelName,  " :You're not on that channel"CRLF);
        return ;
    }
    if (arrayCommand.size() == 2)
    {
        if (this->getChannelByName(channelName)->getTopicName() == "")
        {
            this->sendResponse(fd, ": 331 " + this->getClientByFd(fd)->getNickName() + " #" + channelName + " :No topic is set"CRLF);
            return ;
        }
        size_t pos = this->getChannelByName(channelName)->getTopicName().find(":");
        if (this->getChannelByName(channelName)->getTopicName() != "" && pos == std::string::npos)
        {
            this->sendResponse(fd,": 332" 
                                + this->getClientByFd(fd)->getNickName() 
                                + " #" + channelName 
                                + " " 
                                + this->getChannelByName(channelName)->getTopicName() 
                                + CRLF);
            this->sendResponse(fd,": 333" 
                                + this->getClientByFd(fd)->getNickName() 
                                + " #" 
                                + channelName 
                                + " " 
                                + this->getClientByFd(fd)->getNickName() 
                                + " " 
                                + this->getChannelByName(channelName)->getTime() 
                                + CRLF);
            return ;
        }
        else
        {
            size_t pos = this->getChannelByName(channelName)->getTopicName().find(" ");

            if (pos == 0)
                this->getChannelByName(channelName)->getTopicName().erase(0, 1);
            this->sendResponse(fd,": 332" 
                                + this->getClientByFd(fd)->getNickName() 
                                + " #" + channelName 
                                + " " 
                                + this->getChannelByName(channelName)->getTopicName() 
                                + CRLF);
            this->sendResponse(fd,": 333" 
                                + this->getClientByFd(fd)->getNickName() 
                                + " #" 
                                + channelName 
                                + " " 
                                + this->getClientByFd(fd)->getNickName() 
                                + " " 
                                + this->getChannelByName(channelName)->getTime() 
                                + CRLF);
            return ;
        }
    }
    if (arrayCommand.size() >= 3)
    {
        std::vector<std::string> tmp;
        int pos = getPos(command);
        if (pos == -1 || arrayCommand[2][0] != ':')
        {
            tmp.push_back(arrayCommand[0]);
            tmp.push_back(arrayCommand[1]);
            tmp.push_back(arrayCommand[2]);
        }
        else
        {
            tmp.push_back(arrayCommand[0]);
            tmp.push_back(arrayCommand[1]);
            tmp.push_back(command.substr(getPos(command)));
        }

        if (tmp[2][0] == ':' && tmp[2][1] == '\0')
        {
            this->sendError(fd, 331, "#" + channelName, " :No topic is set"CRLF);
            return ;
        }
        if (this->getChannelByName(channelName)->getTopicRestriction() && this->getChannelByName(channelName)->getClient(fd))
        {
            this->sendError(fd, 482, "#" + channelName, " :You're Not a channel operator"CRLF);
            return ;
        }
        else if (this->getChannelByName(channelName)->getTopicRestriction() && this->getChannelByName(channelName)->getAdmin(fd))
        {
            this->getChannelByName(channelName)->setTime(timeTopic());
            this->getChannelByName(channelName)->setTopicName(tmp[2]);
            std::string rpl;
            size_t pos = tmp[2].find(":");
            if (pos == std::string::npos)
                rpl = ":" 
                    + this->getClientByFd(fd)->getNickName() 
                    + "!" 
                    + this->getClientByFd(fd)->getUserName() 
                    + "@localhost TOPIC #" 
                    + channelName 
                    + " :" 
                    + this->getChannelByName(channelName)->getTopicName()
                    + CRLF;
            else
                rpl = ":" 
                    + this->getClientByFd(fd)->getNickName() 
                    + "!" 
                    + this->getClientByFd(fd)->getUserName() 
                    + "@localhost TOPIC #" 
                    + channelName 
                    + " " 
                    + this->getChannelByName(channelName)->getTopicName()
                    + CRLF;
            this->getChannelByName(channelName)->sendToAll(rpl);
        }
        else
        {
            std::string rpl;
            size_t pos = tmp[2].find(":");

            if (pos == std::string::npos)
            {
                this->getChannelByName(channelName)->setTime(timeTopic());
                this->getChannelByName(channelName)->setTopicName(tmp[2]);
                rpl = ":"
                    + this->getClientByFd(fd)->getNickName() 
                    + "!" 
                    + this->getClientByFd(fd)->getUserName() 
                    + "@localhost TOPIC #" 
                    + channelName 
                    + " " 
                    + this->getChannelByName(channelName)->getTopicName()
                    + CRLF;
            }
            else
            {
                size_t pos2 = tmp[2].find(" ");

                this->getChannelByName(channelName)->setTopicName(tmp[2]);
                if (pos2 == std::string::npos && tmp[2][0] == ':' && tmp[2][1] != ':')
                    tmp[2] = tmp[2].substr(1);
                this->getChannelByName(channelName)->setTopicName(tmp[2]);
                this->getChannelByName(channelName)->setTime(timeTopic());
                rpl = ":"
                    + this->getClientByFd(fd)->getNickName()
                    + "!"
                    + this->getClientByFd(fd)->getUserName()
                    + "@localhost TOPIC #" 
                    + channelName 
                    + " " 
                    + this->getChannelByName(channelName)->getTopicName()
                    + CRLF;
            }
            this->getChannelByName(channelName)->sendToAll(rpl);
        }
    }
}