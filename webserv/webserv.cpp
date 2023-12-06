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
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = Accept(server_pollfd.fd, (struct sockaddr *)&client_addr, &client_len);
    struct pollfd pollfd;
    pollfd.fd = client_fd;
    pollfd.events = POLLIN;
    pollfds.push_back(pollfd);
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
    if ((pollfd.revents & (POLLERR | POLLNVAL)) || !(pollfd.revents & POLLIN) && (pollfd.revents & POLLHUP))
    {
        //remove client here
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
        if (!pollRevents())
            continue;
        for (size_t i = 0; i < pollfds.size(); i++)
        {
            pollError(pollfds[i], i);
            if(pollfds[i].revents & POLLIN)
            {
                if (isServer(pollfds[i].fd))
                    addNewClient(pollfds[i]);
                else
                {
                    //read from client
                    //parse request
                    //create response
                }
            }
            else if (pollfds[i].revents & POLLOUT)
                //send response

        }
    }
}