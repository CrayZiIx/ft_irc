#pragma once

/******************************************************************/
/********************|COLORS-DEFINE           |********************/
/******************************************************************/

#define RED     "\e[1;31m" //-> for red color
#define WHITE   "\e[0;37m" //-> for white color
#define GREEN   "\e[1;32m" //-> for green color
#define YELLOW  "\e[1;33m" //-> for yellow color

/******************************************************************/
/********************|INCLUDE PARTS           |********************/
/******************************************************************/

#include "defineMessage.hpp"
#include "utils.h"

#include <iostream>
#include <vector>
#include <csignal>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

/******************************************************************/
/********************|CLASS DEFINITION        |********************/
/******************************************************************/

class Client
{
    private:
        int _fd;
        bool _operator;
        bool _registered;
        bool _logedin;
        std::string _userName;
        std::string _nickName;
        std::string _buffer;
        std::string _ipAddr;
        std::vector<std::string> channelInvite;
    public:
        Client();
        Client(const Client &other);
        Client(std::string nickname, std::string username, int fd);
        ~Client();
        Client &operator=(const Client &other);


        int getClientFd();
        bool getRegistered();
        bool getInviteChannel(std::string &channelName);
        bool getLogedIn();
        std::string getBuffer();
        std::string getNickName();
        std::string getUserName();
        std::string getIpAddr();
        std::string getHostname();

        void setClientFd(int fd);
        void setUsername(std::string &userName);
        void setNickname(std::string &nickName);
        void setLogedIn(bool value);
        void setBuffer(std::string recived);
        void setRegistered(bool value);
        void setClientIpAddr(std::string ipAddr);

        void clearBuffer();
        void addChannelInvite(std::string &channelName);
        void removeChannelInvite(std::string &channelName);
};