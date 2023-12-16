#ifndef CGI_HPP
#define CGI_HPP

#include "../webserv/webserv.hpp"

class cgi
{
    int pipefd[2];
    pid_t cpid;
    char buf;
    std::string compiler;
    std::string scriptPath;
    char **envp;
    std::string request;
    std::string body;
    std::string header;
    std::string response;
    char **argv;

    public:
        cgi (std::string _compiler, std::string _scriptPath, std::string _request);
        ~cgi();
        void excuteCgi();
        void setEnvp();
        std::string getResponse();
};
#endif