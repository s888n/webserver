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
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <cerrno>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <vector>

#include "../server/server.hpp"
#include "../utils/utils.hpp"
#include "../parser/parser.hpp"

#define TIMEOUT 1000
#define BUFFER_SIZE 1024

class server;

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