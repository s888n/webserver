#ifndef CGI_HPP
#define CGI_HPP

#include "../webserv/webserv.hpp"

class cgi
{
    int *pipe_fd;
    char **env;
    char **argv;
    int pid;
    int status;
    std::string path;
    std::string execute(std::string &compiler, std::string &scriptPath);
};
#endif