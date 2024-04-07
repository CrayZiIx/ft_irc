#include "../inc/Channel.hpp"

Channel::Channel()
{
    return ;
}

Channel::~Channel()
{
    return ;
}

Channel::Channel(const Channel &other)
{
    *this = other;
    return ;
}

Channel &Channel::operator=(const Channel &other)
{
    this->_invitOnly = other._invitOnly;
    this->_topic = other._topic;
    this->_key = other._key;
    this->_limit = other._limit;
    this->_topicRestriction = other._topicRestriction;
    this->_name = other._name;
    this->_password = other._password;
    this->_createdAt = other._createdAt;
    this->_topicName = other._topicName;
    this->_clients = other._clients;
    this->_admins = other._admins;
    this->_modes = other._modes;
    return (*this);
}

void Channel::setInvitationOnly(int invite)
{
    this->_invitOnly = invite;
    return ;
}

void Channel::setTopic(int topic)
{
    this->_topic = topic;
    return ;
}

void Channel::setKey(int key)
{
    this->_key = key;
    return ;
}

void Channel::setLimit(int limit)
{
    this->_limit = limit;
    return ;
}

void Channel::setTopicName(std::string topicName)
{
    this->_topicName = topicName;
    return ;
}

void Channel::setPassword(std::string password)
{
    this->_password = password;
    return ;
}

void Channel::setName(std::string name)
{
    this->_name = name;
    return ;
}

void Channel::setTime(std::string time)
{
    this->_timeCreation = time;
    return ;
}

void Channel::setTopicRestriction(bool value)
{
    this->_topicRestriction = value;
    return ;
}

void Channel::setModeAtIndex(size_t index, bool mode)
{
    this->_modes[index].second = mode;
    return ;
}

void Channel::setCreationTime()
{
    std::time_t _time = std::time(NULL);
    std::ostringstream stringStream;

    stringStream<<_time;
    this->_createdAt = std::string(stringStream.str());
    return ;
}

int Channel::getInvitOnly()
{
    return (this->_invitOnly);
}

int Channel::getTopic()
{
    return (this->_topic);
}

int Channel::getKey()
{
    return (this->_key);
}

int Channel::getLimit()
{
    return (this->_limit);
}

int Channel::getClientsNumber()
{
    return (this->_clients.size() + this->_admins.size());
}

bool Channel::getTopicRestriction() const
{
    return (this->_topicRestriction);
}

bool Channel::getModeAtIndex(size_t index)
{
    return (_modes[index].second);
}

bool Channel::clientInChannel(std::string &nick)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i].getNickName() == nick)
            return (true);
    }
    for (size_t i = 0; i < _admins.size(); i++)
    {
        if (_admins[i].getNickName() == nick)
            return (true);
    }
    return (false);
}

std::string Channel::getTopicName()
{
    return (this->_topicName);
}

std::string Channel::getPassword()
{
    return (this->_password);
}

std::string Channel::getName()
{
    return (this->_name);
}

std::string Channel::getTime()
{
    return (this->_timeCreation);
}

std::string Channel::getTimeCreation()
{
    return (this->_createdAt);
}

std::string Channel::getModes()
{
    std::string mode;
    for (size_t i = 0; i < this->_modes.size(); i++)
    {
        if (this->_modes[i].first != 'o' && this->_modes[i].second)
            mode.push_back(this->_modes[i].first);
    }
    if (!mode.empty())
        mode.insert(mode.begin(), '+');
    return (mode);
}

std::string Channel::clientChannelList()
{
    std::string list;
    
    for (size_t i = 0; i < this->_admins.size(); i++)
    {
        list += "@" + this->_admins[i].getNickName();
        if ((i + 1) < this->_admins.size())
            list += " "; 
    }
    if (this->_clients.size())
        list += " ";
    for (size_t i = 0; i < this->_clients.size(); i++)
    {
        list += this->_clients[i].getNickName();
        if ((i + 1) < this->_clients.size())
            list += " ";
    }
    return (list);
}

Client *Channel::getClient(int fd)
{
    std::vector<Client>::iterator it = this->_clients.begin();
    for (; it != this->_clients.end(); it++)
    {
        if (it->getClientFd() == fd)
            return (&(*it));
    }
    return (NULL);
}

Client *Channel::getAdmin(int fd)
{
    std::vector<Client>::iterator it = this->_admins.begin();
    for (; it != this->_admins.end(); it++)
    {
        if (it->getClientFd() == fd)
            return (&(*it));
    }
    return (NULL);
}

Client *Channel::getClientInChannel(std::string name)
{
    std::vector<Client>::iterator clientIt = this->_clients.begin();
    std::vector<Client>::iterator adminIt = this->_admins.begin();
    for (; clientIt != this->_clients.end(); clientIt++)
    {
        if (clientIt->getNickName() == name)
            return (&(*clientIt));
    }
    for (; adminIt != this->_admins.end(); adminIt++)
    {
        if (adminIt->getNickName() == name)
            return (&(*adminIt));
    }
    return (NULL);
}

void Channel::addClient(Client newClient)
{
    this->_clients.push_back(newClient);
}

void Channel::addAdmin(Client newClient)
{
    this->_admins.push_back(newClient);
}

void Channel::removeClient(int fd)
{
    std::vector<Client>::iterator clientIt = this->_clients.begin();
    for (; clientIt != this->_clients.end(); clientIt++)
    {
        if (clientIt->getClientFd() == fd)
        {
            this->_clients.erase(clientIt);
            break ;
        }
    }
}

void Channel::removeAdmin(int fd)
{
    std::vector<Client>::iterator adminIt = this->_admins.begin();
    for (; adminIt != this->_admins.end(); adminIt++)
    {
        if (adminIt->getClientFd() == fd)
        {
            this->_admins.erase(adminIt);
            break ;
        }
    }   
}

bool Channel::changeClientToAdmin(std::string &nickName)
{
    size_t i = 0;
    for (; i < this->_clients.size(); i++)
    {
        if (this->_clients[i].getNickName() == nickName)
            break ;
    }
    if (i < this->_clients.size())
    {
        this->_admins.push_back(this->_clients[i]);
        this->_clients.erase(this->_clients.begin() + i);
        return (true);
    }
    return (false);
}

bool Channel::changeAdminToClient(std::string &nickName)
{
    size_t i = 0;
    for (; i < this->_admins.size(); i++)
    {
        if (this->_admins[i].getNickName() == nickName)
            break ;
    }
    if (i < this->_admins.size())
    {
        this->_clients.push_back(this->_admins[i]);
        this->_admins.erase(this->_admins.begin() + i);
        return (true);
    }
    return (false);
}

void Channel::sendToAll(std::string rpl1)
{
    for (size_t i = 0; i < this->_admins.size(); i++)
    {
        if (send(this->_admins[i].getClientFd(), rpl1.c_str(), rpl1.size(), 0) == -1)
            std::cerr<<"send() has faild"<<std::endl;
    }
    for (size_t i = 0; i < this->_clients.size(); i++)
    {
        if (send(this->_clients[i].getClientFd(), rpl1.c_str(), rpl1.size(), 0) == -1)
            std::cerr<<"send() has faild"<<std::endl;
    }

}

void Channel::sendToAll(std::string rpl1, int fd)
{
    for (size_t i = 0; i < this->_admins.size(); i++)
    {
        if (this->_admins[i].getClientFd() != fd)
            if (send(this->_admins[i].getClientFd(), rpl1.c_str(), rpl1.size(), 0) == -1)
                std::cerr<<"send() faild"<<std::endl;
    }
    for (size_t i = 0; i < this->_clients.size(); i++)
    {
        if (this->_clients[i].getClientFd() != fd)
            if (send(this->_clients[i].getClientFd(), rpl1.c_str(), rpl1.size(), 0) == -1)
                std::cerr<<"send() faild"<<std::endl;
    }
}