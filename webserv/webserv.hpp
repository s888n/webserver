#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../server/server.hpp"
#include "../utils/utils.hpp"
#include "../parser/parser.hpp"

#define TIMEOUT 1000
#define BUFFER_SIZE 1024

class webserv
{
    public:
        char buffer[BUFFER_SIZE];
        std::vector<server> servers;
        std::vector<struct pollfd> pollfds;
        webserv(std::string &filename);
        void init();
        void run();
};
#endif