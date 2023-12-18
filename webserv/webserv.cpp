#include "webserv.hpp"

webserv::webserv(std::string &filename)
{
    parser parser(filename);
    servers = parser.getServers();
}

void webserv::init()
{
    for (size_t i = 0;i < servers.size(); i++)
    {
        struct pollfd pollfd;
        servers[i].createSocket();
        pollfd.fd = servers[i].getSocket();
        pollfd.events = POLLIN;
        pollfds.push_back(pollfd);
    }
}

void webserv::addNewClient(struct pollfd &server_pollfd)
{
    Client client;
    client.addrlen = sizeof(client.addr);
    client._servers = &servers;
    client._socket  = Accept(server_pollfd.fd, (struct sockaddr *)&client.addr, &client.addrlen);
    if(client._socket == -1 )
        return;
    client.timestamp = getTime();
    Fcntl(client._socket, F_SETFL, O_NONBLOCK);
    client.serverFd = server_pollfd.fd;
    struct pollfd pollfd;
    pollfd.fd = client._socket;
    pollfd.events = POLLIN;
    pollfds.push_back(pollfd);
    clients.push_back(client);
}
void webserv::writeToClient(struct pollfd &pollfd)
{  
    Client *client = getClient(pollfd.fd);
    if(client == NULL)
        return;
        client->sendResponse();
    if(client->getIsBodyEnd() == true)
        closeClient(pollfd.fd);
}

void webserv::readFromClient(struct pollfd &pollfd)
{
    // (void)pollfd;
    Client* client = getClient(pollfd.fd);
    try
    {
        if (client == NULL)
            return;
        client->readRequest();
        if(client->getIsReadBody() == false && client->getIsParsed() == true && client->_isCgi == false)
        {
            pollfd.events = POLLOUT;
            client->_locationResponse = client->_location;
            if(client->_locationResponse->isCgi == true)
            {
                client->_bodyResponse = client->_body;
                client->isBodyString = true;
                client->_file = "/";
            }
        }

    }catch(const char *e)
    {
        std::string tmp;
        pollfd.events = POLLOUT;

        client->_file = client->_pathFile;
        client->_locationResponse = client->_location;
        if(client->checkReturn() == true)
        {
            client->_file = "/";
            client->isBodyString = true;
            client->_bodyResponse = client->_body;
            return;
        }

        if(client->_server->error_pages.find(client->getErrorCode()) == client->_server->error_pages.end() )
        {
            if(client->_isError == true)
                client->_file = "/";
            client->isBodyString = true;
        }
        else
            client->_file = client->_server->error_pages[client->getErrorCode()];
    }
}

bool webserv::pollRevents()
{
    int ret = Poll(&pollfds[0], pollfds.size(), -1);
    if (ret <= 0)
        return false;
    return true;
}

void webserv::pollError(struct pollfd &pollfd, size_t &i)
{
    if ((pollfd.revents & (POLLERR | POLLNVAL)) || (!(pollfd.revents & POLLIN) && (pollfd.revents & POLLHUP)))
    {
        //reemove client
        close(pollfd.fd);
        pollfds.erase(pollfds.begin() + i);
        i--;
    }
}

bool webserv::isServer(int fd)
{
    for (size_t i = 0; i < servers.size(); i++)
        if (servers[i].getSocket() == fd)
            return true;
    return false;
}

void webserv::run()
{
    while (true)
    {
        pollRevents();
 
        for (size_t i = 0; i < pollfds.size() ; i++)
        {
            //pollError(pollfds[i], i);
            if(pollfds[i].revents == 0)
                continue;
            if(pollfds[i].revents & POLLIN)
            {
                if (isServer(pollfds[i].fd))
                    addNewClient(pollfds[i]);
                else
                    readFromClient(pollfds[i]);
            }
            else if (pollfds[i].revents & POLLOUT)
                writeToClient(pollfds[i]);
            // else
            //     closeClient(pollfds[i].fd);
        }
        checkTimeout();
    }
}
Client *webserv::getClient(int fd)
{
    for (size_t i = 0; i < clients.size(); i++)
        if (clients[i]._socket == fd)
            return &clients[i];
    return NULL;
} 

void webserv::closeClient(int fd)
{
    for (size_t i = 0; i < clients.size(); i++)
        if (clients[i]._socket == fd)
        {
            clients.erase(clients.begin() + i);
            break;
        }
    for(size_t i = 0; i < pollfds.size(); i++)
        if(pollfds[i].fd == fd)
        {
            close(fd);
            pollfds.erase(pollfds.begin() + i);
            break;
        }
}

void webserv::checkTimeout()
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (getTime() - clients[i].timestamp > TIMEOUT)
        {
            std::cout << "timeout" << std::endl;
            closeClient(clients[i]._socket);
        }
    }
}