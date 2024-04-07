#pragma once

#include "socketServer.hpp"
#include "socketClient.hpp"

class Client;

class Channel
{
    private:
        int _invitOnly;
        int _topic;
        int _key;
        int _limit;
        bool _topicRestriction;
        std::string _name;
        std::string _timeCreation;
        std::string _password;
        std::string _createdAt;
        std::string _topicName;
        std::vector<Client> _clients;
        std::vector<Client> _admins;
        std::vector<std::pair<char, bool> > _modes;
    public:
        Channel();
        ~Channel();
        Channel(const Channel &other);
        Channel &operator=(const Channel &other);

        void setInvitationOnly(int invite);
        void setTopic(int topic);
        void setKey(int key);
        void setLimit(int limit);
        void setTopicName(std::string topicName);
        void setPassword(std::string password);
        void setName(std::string name);
        void setTime(std::string time);
        void setTopicRestriction(bool value);
        void setModeAtIndex(size_t index, bool mode);
        void setCreationTime();

        int getInvitOnly();
        int getTopic();
        int getKey();
        int getLimit();
        int getClientsNumber();
        bool getTopicRestriction() const;
        bool getModeAtIndex(size_t index);
        bool clientInChannel(std::string &nick);
        std::string getTopicName();
        std::string getPassword();
        std::string getName();
        std::string getTime();
        std::string getTimeCreation();
        std::string getModes();
        std::string clientChannelList();
        Client *getClient(int fd);
        Client *getAdmin(int fd);
        Client *getClientInChannel(std::string name);

        void addClient(Client newClient);
        void addAdmin(Client newClient);
        void removeClient(int fd);
        void removeAdmin(int fd);
        bool changeClientToAdmin(std::string &nickName);
        bool changeAdminToClient(std::string &nickName);

        void sendToAll(std::string rpl1);
        void sendToAll(std::string rpl1, int fd);
        

};