#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <poll.h>

// #include "../webserv/webserv.hpp"
#include <utility>

class location
{
    private: 
    public:
        bool autoindex;
        std::pair <int,std::string>_return;
        bool isReturn;
        bool isCgi;
        std::string path;
        std::string compiler;
        std::string cgi_path;
        size_t max_body_size;
        std::string root;
        std::vector<std::string> methods;
        std::vector<std::string> indexes;
        std::map <int, std::string> error_pages;
};

#endif