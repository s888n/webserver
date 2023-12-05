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

void webserv::run()
{
    while (true)
    {
        std::cout << "polling" << std::endl;
        exit(0);
    }
}