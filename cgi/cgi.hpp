#ifndef CGI_HPP
#define CGI_HPP

#include "../webserv/webserv.hpp"

char **mapToEnvp(std::map<std::string, std::string> &env);

std::map<std::string, std::string> getHeaders(std::string headers);

class cgi
{
    std::string compiler;
    std::string scriptPath;
    char **envp;
    std::string request;
    std::string header;
    std::string body;
    std::string response;
    char **argv;

public:
    cgi(std::string _compiler, std::string _scriptPath, std::string _request);
    ~cgi();
    void excuteCgi();
    void parseEnv(std::string request);
    std::string getResponse();
};
#endif