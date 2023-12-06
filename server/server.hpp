#ifndef SERVER_HPP
#define SERVER_HPP


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

#include "../webserv/webserv.hpp"
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

class server
{
    private:
    public:
        int port;
        int fd;
        bool autoindex;
        size_t max_body_size;
        struct sockaddr_in addr;
        std::string host;
        std::string root;
        std::vector<std::string> indexes;
        std::vector<std::string> methods;
        std::vector<std::string> server_names;
        std::map <int, std::string> error_pages;
        std::vector<location> locations;

        int getPort() const;
        int getSocket()const;
        bool getAutoindex()const;
        size_t getMaxBodySize()const;
        struct sockaddr_in getAddr()const;
        std::string getHost()const;
        std::string getRoot()const;
        std::vector<std::string> getIndexes()const;
        std::vector<std::string> getMethods()const;
        std::vector<std::string> getServerNames()const;
        std::map <int, std::string> getErrorPages()const;
        std::vector<location> getLocations()const;
        void createSocket(void);
};
#endif