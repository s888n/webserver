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

//class location
//class server
//class request
//class response
//class client
class location
{
    private:
        bool autoindex;
        size_t max_body_size;
        std::string path;
        std::string root;
        std::vector<std::string> methods;
        std::vector<std::string> indexes;
        std::string _return;
        bool isCgi;
};

class server
{
    private:
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
    public:

        void setPort(int port);
        void setSocket(int socket);
        void setAutoindex(bool autoindex);
        void setMaxBodySize(size_t max_body_size);
        void setAddr(struct sockaddr_in addr);
        void setHost(std::string host);
        void setRoot(std::string root);
        void setIndexes(std::vector<std::string> indexes);
        void setMethods(std::vector<std::string> methods);
        void setServerNames(std::vector<std::string> server_names);
        void setErrorPages(std::map <int, std::string> error_pages);
        void setLocations(std::vector<location> locations);
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