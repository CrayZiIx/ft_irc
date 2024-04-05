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

#include "socketClient.hpp"

#include <iostream>
#include <vector>
#include <csignal>
#include <cstring>

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

class Server
{
    private:
        int _servPort;                          // SP
        int _servSocketFd;                      // SSFD
        static bool __signal;
        std::vector<Client> _clients;           // vector of clients
        std::vector<struct pollfd> _pollFd;     // vector of pollfd
    public:
        Server();                           
        ~Server();

        void serverInit();                      // server Init
        void serverSocket();                    // SS creation
        void acceptNewClient();                 // Function to accept a new client
        void receiveNewData(int fd);            // receive new data from a registered client

        static void _signalHandler(int signum); // A basic signal handler

        void closeFds();                        // close Fds, lol
        void clearClients(int fd);              // Clear all Client
};
