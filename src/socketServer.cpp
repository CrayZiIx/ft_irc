#include "../inc/socketServer.hpp"

/******************************************************************/
/********************|STATIC DEFINITION       |********************/
/******************************************************************/

bool Server::__signal = false;

Server::Server()
{
    return ;
}

Server::~Server()
{
    return ;
}

/******************************************************************/
/********************|FIX THIS FUNCTION        ********************/
/******************************************************************/
/*                                                                */
/* don't detect correctly event receivd by poll                   */
/*                                                                */
/******************************************************************/
// send client fd

void Server::serverInit(char **args)                       // server Init
{
    this->_servPassword = args[2];
    this->_servPort = to_int(args[1]);
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
    //     wait some cmd from the usr [CAP LS - PASS - NICK - USER]
    //         if passw isn't
    //             go fuck his mom
    newPoll.fd = incoFd;
    newPoll.events = POLLIN;
    newPoll.revents = 0;
    oneClient.setFd(incoFd);
    oneClient.setIpAddr(inet_ntoa((add.sin_addr)));
    _clients.push_back(oneClient);
    _pollFd.push_back(newPoll);

    std::cout<<GREEN<<"Client ["<<incoFd<<"] has been connected"<<WHITE<<std::endl;
    return ;
}

void Server::receiveNewData(int fd)             // receive new data from a registered client
{
    char buff[1024];
    ssize_t bytes;

    memset(buff, 0, sizeof(buff));
    bytes = recv(fd, buff, sizeof(buff) - 1, 0); // receive the data
    if (bytes == 0)
    {
        std::cout<<RED<<"Client ["<<fd<<"] has been disconnected"<<WHITE<<std::endl;
        for (size_t i = 0; i < _pollFd.size(); i++)
            if (_pollFd[i].fd == fd)
                this->_pollFd.erase(this->_pollFd.begin() + i);
        clearClients(fd);
        close(fd);
    }
    else if (bytes != -1)
    {
        buff[bytes] = '\0';
        std::cout<<YELLOW<<"Client ["<<fd<<"] Data :"<<WHITE<<buff;
        /*HERE PARSE CHECK AUTH HANDLE CMD*/
    }
    return ;
}

void Server::_signalHandler(int signum)         // A basic signal handler
{
    (void)signum;
    std::cout<<std::endl<<"Signal Received !"<<std::endl;
    Server::__signal = true;
    return ;
}

void Server::closeFds()                        // close Fds, lol
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        close(_clients[i].getFd());
        if (_servSocketFd != 1)
        {
            std::cout<<RED<<"Client ["<<_clients[i].getFd()<<"] : has been disconnected"<<WHITE<<std::endl;
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
        if (_clients[i].getFd() == fd)
        {
            _clients.erase(_clients.begin() + i);
            break;
        }
    }
    return ;
}