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

#include "utils.h"


/******************************************************************/
/********************|CLASS DEFINITION        |********************/
/******************************************************************/

class Client
{
    private:
        int _fd;
        std::string _ipAddr;
    public:
        Client();
        ~Client();

        int getFd();
        void setFd(int fd);
        void setIpAddr(std::string ipAddr);
};