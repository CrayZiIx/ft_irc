#pragma once

/******************************************************************/
/********************|INCLUDE PARTS           |********************/
/******************************************************************/

#include "defineMessage.hpp"
#include "socketClient.hpp"
#include "Channel.hpp"

#include <iostream>
#include <vector>
#include <sstream>
#include <csignal>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <ctime>

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

class Channel;

class Server
{
    private:
        int _servPort;                          // server port
        std::string _servPassword;              // server password
        int _servSocketFd;                      // server socket fd ( 3 cassualy)
        std::vector<Client> _clients;           // vector of clients
    	std::vector<Channel> _channels;
        std::vector<struct pollfd> _pollFd;     // vector of pollfd
        static bool __signal;                   
    public:
        Server(); 
        Server(const Server &other);                          
        ~Server();
        Server &operator=(const Server &other);

        void serverInit(char **args);           // server Init
        void serverSocket();                    // SS creation
        void acceptNewClient();                 // Function to accept a new client
        void receiveNewData(int fd);            // receive new data from a registered client

        int getServerPort();
        int getServerFd();
        std::string getServerPassword();
        Client *getClientByFd(int clientFd);
        Client *getClientByNickname(std::string name);
	    Channel *getChannelByName(std::string channelsName);

        void setServerPort(int serverPort);
        void setServerPassword(std::string serverPassword);
        void setServerFd(int serverSocketFd);
         
        void addClient(Client newClient);
        void addSocketFd(struct pollfd newSocketFd); 
        void addChannel(Channel newChannel);
        void removeClient(int clientFd);
        void removeSocketFd(int socketFd);
        void removeChannelByName(std::string channelName);
        void removeChannels(int socketChannel);

        void sendError(int fd, int code, std::string clientName, std::string message);
        void sendError(int fd, int code, std::string clientName, std::string channelName, std::string message);
        void sendResponse(int fd, std::string response);

        std::vector<std::string> splitRecivedBuffer(std::string str);
        std::vector<std::string> splitCommand(std::string &command);
        void parseExecCommand(std::string &command, int fd);

        /*AUTHENTICATE*/
        bool notRegistered(int fd);
	    bool isValidNickname(std::string& nickname);
	    bool nickNameInUser(std::string& nickname);
        void setNickname(std::string command, int fd);
        void setUsername(std::string &command, int fd);
	    void authentifcateClient(int fd, std::string command);

        void joinCommand(std::string command, int fd);
        int joinSplit(std::vector<std::pair<std::string, std::string> > &token, std::string command, int fd);
        int searchClientInChannel(std::string nickName);
        void channelIsExisting(std::vector<std::pair<std::string, std::string> > &token, int i, int j, int fd);
        void channelIsntExisting(std::vector<std::pair<std::string, std::string> > &token, int i, int fd);

        void inviteCommand(std::string &command, int &fd);
        void quitCommand(std::string command, int fd);

        static void _signalHandler(int signum); // A basic signal handler

        void closeFds();                        // close Fds, lol
        void clearClients(int fd);              // Clear all Client
};
