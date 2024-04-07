#include "../inc/socketServer.hpp"

/******************************************************************/
/********************|STATIC DEFINITION       |********************/
/******************************************************************/

bool Server::__signal = false;

void Server::_signalHandler(int signum)         // A basic signal handler
{
    (void)signum;
    std::cout<<std::endl<<"Signal Received !"<<std::endl;
    Server::__signal = true;
    return ;
}
/******************************************************************/
/********************|COPLIEN FUNCTION        |********************/
/******************************************************************/
Server::Server()
{
    this->_servSocketFd = -1;
    return ;
}

Server::~Server()
{
    return ;
}

Server::Server(const Server &other)
{
    *this = other;
    return ;
}                 


Server &Server::operator=(const Server &other)
{
    this->__signal = other.__signal;
    this->_clients = other._clients;
    this->_pollFd = other._pollFd;
    /*todo: add channel*/
    this->_servPort = other._servPort;
    this->_servPassword = other._servPassword;
    this->_servSocketFd = other._servSocketFd;
    return (*this);
}

/******************************************************************/
/********************|METHODE                 |********************/
/******************************************************************/
/********************|server function         |********************/
/******************************************************************/

void Server::serverInit(char **args)                       // server Init
{
    this->setServerPassword(args[2]);
    this->setServerPort(to_int(args[1]));
    this->serverSocket();

    std::cout<<"Waiting a connection..."<<std::endl;

    while (Server::__signal == false)
    {
        if ((poll(&_pollFd[0], _pollFd.size(), -1) == -1) && Server::__signal == false) // Wainting for a event
            throw(std::runtime_error("poll() has failed"));
        for (size_t i = 0; i < this->_pollFd.size(); i++)
        {
            if (_pollFd[i].events & POLLIN) // check if there is data to read
            {
                if (_pollFd[i].fd == _servSocketFd)
                    this->acceptNewClient(); // accept new client 
                else
                    this->receiveNewData(_pollFd[i].fd); // receive new data from a registered client
            }
        }
    }
    this->closeFds(); // close the file descriptors when the server stops x)
    return ;
}

void Server::serverSocket()                     // SS creation
{
    struct sockaddr_in add;
    struct pollfd newPoll;

    int en = 1;

    add.sin_family = AF_INET;                   // Set the address family to Ipv4
    add.sin_port = htons(this->_servPort);      // Convert the port network byte order (bid endian)
    add.sin_addr.s_addr = INADDR_ANY;           // Set the address to any local machine adress
    
    this->_servSocketFd = socket(AF_INET, SOCK_STREAM, 0);  // Create the server socket ? check socket function
    if (this->_servSocketFd == -1)
        throw (std::runtime_error("Failed to Create Socket"));
    if (setsockopt(this->_servSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) // Set the Socket Option (SO_REUSEADDR) to reuse the address
        throw (std::runtime_error("Failed to set option (SO_REUSEADDR) on socket"));
    if (fcntl(this->_servSocketFd, F_SETFL, O_NONBLOCK) == -1) // Set the Socket Option (O_NONBLOCK) for non-blocking socket
        throw (std::runtime_error("Failed to set option (SO_NONBLOCK) on socket"));
    if (bind(this->_servSocketFd, (struct sockaddr *)&add, sizeof(add)) == -1)  // Bind the Socket to the Address
        throw (std::runtime_error("Failed to bind Socket"));
    if (listen(this->_servSocketFd, SOMAXCONN) == -1)       // listen for incoming connections and making the socket a passive socket
        throw (std::runtime_error("listen() has failed"));

    newPoll.fd = this->_servSocketFd;
    newPoll.events = POLLIN;
    newPoll.revents = 0;
    this->_pollFd.push_back(newPoll);
    return ;
}  

void Server::acceptNewClient()                  // Function to accept a new client
{
    Client oneClient;
    struct sockaddr_in add;
    struct pollfd newPoll;
    socklen_t len = sizeof(add);

    int incoFd = accept(this->_servSocketFd, (sockaddr *)&(add), &len);
    if (incoFd == -1)
    {
        std::cout<<"accept() has failed !"<<std::endl;
        return ;
    }
    else if (fcntl(incoFd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cout<<"fcntl() has failed !"<<std::endl;
        return ;
    }
    // TODO:
    //     wait some cmd from the usr [CAP LS - PASS - NICK - USER], those need to be in this order !
    //         if passw isn't
    //             go fuck his mom
    newPoll.fd = incoFd;
    newPoll.events = POLLIN;
    newPoll.revents = 0;
    oneClient.setClientFd(incoFd);
    oneClient.setClientIpAddr(inet_ntoa((add.sin_addr)));
    _clients.push_back(oneClient);
    _pollFd.push_back(newPoll);

    std::cout<<GREEN<<"Client ["<<incoFd<<"] has been connected"<<WHITE<<std::endl;
    return ;
}

void Server::receiveNewData(int fd)             // receive new data from a registered client
{
    std::vector<std::string> command;
    char buff[1024];
    Client *cli = getClientByFd(fd);
    ssize_t bytes;

    memset(buff, 0, sizeof(buff));
    bytes = recv(fd, buff, sizeof(buff) - 1, 0); // receive the data
    if (bytes == 0)
    {
        std::cout<<RED<<"Client ["<<fd<<"] has been disconnected"<<WHITE<<std::endl;
        // for (size_t i = 0; i < _pollFd.size(); i++)
        //     if (_pollFd[i].fd == fd)
        //         this->_pollFd.erase(this->_pollFd.begin() + i);
        clearClients(fd);
        removeChannels(fd);
        removeClient(fd);
        removeSocketFd(fd);
        close(fd);
    }
    else if (bytes != -1)
    {
        std::cout<<buff<<std::endl;
        cli->setBuffer(buff);
        if (cli->getBuffer().find_first_of(CRLF) == std::string::npos)
            return ;
        command = splitRecivedBuffer(cli->getBuffer());
        for (size_t i = 0; i < command.size(); i++)
            this->parseExecCommand(command[i], fd);
        if (this->getClientByFd(fd))
            this->getClientByFd(fd)->clearBuffer();
        std::cout<<YELLOW<<"Client ["<<fd<<"] Data :"<<WHITE<<buff;
    }
    return ;
}

/******************************************************************/
/********************|METHODE                 |********************/
/******************************************************************/
/********************|Getters                 |********************/
/******************************************************************/

int Server::getServerPort()
{
    return (this->_servPort);
}

int Server::getServerFd()
{
    return (this->_servSocketFd);
}

std::string Server::getServerPassword()
{
    return (this->_servPassword);
}

Client *Server::getClientByFd(int clientFd)
{
    for (size_t i = 0; i < this->_clients.size(); i++)
    {
        if (clientFd == this->_clients[i].getClientFd())
            return (&this->_clients[i]);
    }
    return (NULL);
}
Client *Server::getClientByNickname(std::string nickName)
{
    for (size_t i = 0; i < this->_clients.size(); i++)
    {
        if (nickName == this->_clients[i].getNickName())
            return (&this->_clients[i]);
    }
    return (NULL);
}

Channel *Server::getChannelByName(std::string channelsName)
{
    for (size_t i = 0; i < this->_channels.size(); i++)
    {
        if (channelsName == this->_channels[i].getName())
            return (&this->_channels[i]);
    }
    return (NULL);
}


/******************************************************************/
/********************|METHODE                 |********************/
/******************************************************************/
/********************|Setters                 |********************/
/******************************************************************/

void Server::setServerPort(int serverPort)
{
    this->_servPort = serverPort;
    return ;
}

void Server::setServerPassword(std::string serverPassword)
{
    this->_servPassword = serverPassword;
    return ;
}

void Server::setServerFd(int serverSocketFd)
{
    this->_servSocketFd = serverSocketFd;
    return ;
}

/******************************************************************/
/********************|METHODE                 |********************/
/******************************************************************/
/********************|Manage Clie./Chan./Sock.|********************/
/******************************************************************/

void Server::addClient(Client newClient)
{
    this->_clients.push_back(newClient);
    return ;
}
void Server::addSocketFd(struct pollfd newSocketFd)
{
    this->_pollFd.push_back(newSocketFd);
    return ; 
}

void Server::addChannel(Channel newChannel)
{
    this->_channels.push_back(newChannel);
    return ; 
}

void Server::removeClient(int clientFd)
{
    for (size_t i = 0; i < this->_clients.size(); i++)
    {
        if (clientFd == this->_clients[i].getClientFd())
        {
            this->_clients.erase(this->_clients.begin() + i);
            return ;
        }
    }
    return ;
}

void Server::removeSocketFd(int socketFd)
{
    for (size_t i = 0; i < this->_pollFd.size(); i++)
    {
        if (socketFd == this->_pollFd[i].fd) 
        {
            this->_pollFd.erase(this->_pollFd.begin() + i);
            return ;
        }
    }
    return ; 
}

void Server::removeChannelByName(std::string channelName)
{
    for (size_t i = 0; i < this->_channels.size(); i++)
    {
        if (channelName == this->_channels[i].getName())
        {
            this->_channels.erase(this->_channels.begin() + i);
            return ;
        }
    }
    return ;
}

void Server::removeChannels(int socketChannel)
{
    int flags = 0;
    for (size_t i = 0; i < this->_channels.size(); i++)
    {
        if (this->_channels[i].getClient(socketChannel))
        {
            this->_channels[i].removeClient(socketChannel);
            flags = 1;
        }
        else if (this->_channels[i].getAdmin(socketChannel))
        {
            this->_channels[i].removeAdmin(socketChannel);
            flags = 1;
        }
        if (this->_channels[i].getClientsNumber() == 0)
        {
            this->_channels.erase(this->_channels.begin() + i--);
            continue ;
        }
        if (flags)
        {
            std::string rpl = ":" + this->getClientByFd(socketChannel)->getNickName() + "!~" + this->getClientByFd(socketChannel)->getUserName() + "@localhost QUIT Quit"CRLF;
            this->_channels[i].sendToAll(rpl);
        }
    }
}

/******************************************************************/
/********************|METHODE                 |********************/
/******************************************************************/
/********************|Sending methode         |********************/
/******************************************************************/

void Server::sendError(int fd, int code, std::string clientName, std::string message)
{
    std::stringstream stringStream;

    stringStream<<":localhost"<<code<<" "<<clientName<<message;
    
    std::string response = stringStream.str();
    
    if (send(fd, response.c_str(), response.size(), 0) == -1)
        std::cerr<<"send() has failed"<<std::endl;
    return ;
}

void Server::sendError(int fd, int code, std::string clientName, std::string channelName, std::string message)
{
    std::stringstream stringStream;

    stringStream<<":localhost"<<code<<" "<<clientName<<" "<<channelName<<message;
    
    std::string response = stringStream.str();
    
    if (send(fd, response.c_str(), response.size(), 0) == -1)
        std::cerr<<"send() has failed"<<std::endl;
    return ;
}

void Server::sendResponse(int fd, std::string response)
{
    std::cout<<"Response:"<<std::endl<<response;
    if (send(fd, response.c_str(), response.size(), 0) == -1)
        std::cerr<<"Response send() has failed"<<std::endl;
    return ;
}

/******************************************************************/
/********************|METHODE                 |********************/
/******************************************************************/
/********************|Parsing and util        |********************/
/******************************************************************/

std::vector<std::string> Server::splitRecivedBuffer(std::string str)
{
    std::vector<std::string> arr;
    std::istringstream stringStream(str);
    std::string line;

    while (std::getline(stringStream, line))
    {
        size_t pos = line.find_first_of(CRLF);
        if (pos != std::string::npos)
            line = line.substr(0, pos);
        arr.push_back(line);
    }
    return (arr);
}

std::vector<std::string> Server::splitCommand(std::string &command)
{
    std::vector<std::string> arr;
    std::istringstream stringStream(command);
    std::string token;

    while (stringStream >> token)
    {
        arr.push_back(token);
        token.clear();
    }
    return (arr);
}

void Server::parseExecCommand(std::string &command, int fd)
{
    if(command.empty())
        return ;
    
    std::vector<std::string> commandSplited = splitCommand(command);
    size_t index_found = command.find_first_not_of(" \t\v");
    if (index_found != std::string::npos)
        command = command.substr(index_found);
    if (commandSplited.size()\
        && (commandSplited[0] == "PASS" || commandSplited[0] == "pass"))
        this->authentifcateClient(fd, command);
    else if (commandSplited.size()\
        && (commandSplited[0] == "NICK" || commandSplited[0] == "nick"))
        this->setNickname(command, fd);// maybe bug here !
    else if (commandSplited.size()\
        && (commandSplited[0] == "USER" || commandSplited[0] == "user"))
        this->setUsername(command, fd);
    else if (commandSplited.size()\
        && (commandSplited[0] == "QUIT" || commandSplited[0] == "QUIT"))
        this->quitCommand(command, fd);
    else if (notRegistered(fd))
    {
        if (commandSplited.size()\
            && (commandSplited[0] == "KICK" || commandSplited[0] == "kick"))
            std::cout<<commandSplited[0]<<std::endl;// kickCommand(command, fd);
        else if (commandSplited.size()\
            && (commandSplited[0] == "JOIN" || commandSplited[0] == "join"))
            this->joinCommand(command, fd);
        else if (commandSplited.size()\
            && (commandSplited[0] == "TOPIC" || commandSplited[0] == "topic"))
            std::cout<<commandSplited[0]<<std::endl;//topicCommand(command, fd);
        else if (commandSplited.size()\
            && (commandSplited[0] == "MODE" || commandSplited[0] == "mode"))
            std::cout<<commandSplited[0]<<std::endl;//modeCommand(command, fd);
        else if (commandSplited.size()\
            && (commandSplited[0] == "PART" || commandSplited[0] == "part"))
            std::cout<<commandSplited[0]<<std::endl;//partCommand(command, fd);
        else if (commandSplited.size()\
            && (commandSplited[0] == "PRIVMSG" || commandSplited[0] == "privmsg"))
            this->privmsgCommand(command, fd);
        else if (commandSplited.size()\
            && (commandSplited[0] == "INVITE" || commandSplited[0] == "invite"))
            this->inviteCommand(command, fd);
        else if (commandSplited.size())
            this->sendResponse(fd, ERR_CMDNOTFOUND(this->getClientByFd(fd)->getNickName(), commandSplited[0]));
    }
    else if (!notRegistered(fd))
        this->sendResponse(fd, ERR_NOTREGISTERED(std::string("*"))); 
    return ;    
}

/******************************************************************/
/********************|METHODE                 |********************/
/******************************************************************/
/********************|exit/close function     |********************/
/******************************************************************/

void Server::closeFds()                        // close Fds, lol
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        close(_clients[i].getClientFd());
        if (_servSocketFd != 1)
        {
            std::cout<<RED<<"Client ["<<_clients[i].getClientFd()<<"] : has been disconnected"<<WHITE<<std::endl;
            close(_servSocketFd);
        }
    }
    return ;
}

void Server::clearClients(int fd)              // Clear all Client
{
    for (size_t i = 0; i < _pollFd.size(); i++)
    {
        if (_pollFd[i].fd == fd)
        {
            _pollFd.erase(_pollFd.begin() + i);
            break;
        }
        if (_clients[i].getClientFd() == fd)
        {
            _clients.erase(_clients.begin() + i);
            break;
        }
    }
    return ;
}