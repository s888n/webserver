#include "cgi.hpp"


cgi::cgi(std::string _compiler, std::string _scriptPath, std::string _request)
{
    if (_compiler == "" || _scriptPath == "" || _request == "")
    {
        perror ("cgi constructor");
        return;
    }

    this->compiler = _compiler;
    this->request = _request;
    this->scriptPath = _scriptPath;
    setEnvp();
    argv = new char *[3];
    
    argv[0] = new char[compiler.size() + 1];
    strcpy(argv[0], compiler.c_str());
    argv[0][compiler.size()] = '\0';
    
    argv[1] = new char[scriptPath.size() + 1];
    strcpy(argv[1], scriptPath.c_str());
    argv[1][scriptPath.size()] = '\0';

    argv[2] = NULL;
    excuteCgi();
}

cgi::~cgi()
{
    for (int i = 0; envp[i] != NULL; i++)
        delete[] envp[i];
    delete[] envp;
    for (int i = 0; argv[i] != NULL; i++)
        delete[] argv[i];
    delete[] argv;
}

void cgi::setEnvp()
{
    std::map<std::string, std::string> env;
    header = request.substr(0, request.find("\r\n\r\n"));
    body = request.substr(request.find("\r\n\r\n") + 4);
    
    //turn header into map
    std::string tmp = header.substr(0, header.find("\r\n"));
    env["REQUEST_METHOD"] = tmp.substr(0, tmp.find(" "));
    header = header.substr(header.find("\r\n") + 2);
    while (header.find("\r\n") != std::string::npos)
    {
        tmp = header.substr(0, header.find("\r\n"));
        env[tmp.substr(0, tmp.find(":"))] = tmp.substr(tmp.find(":") + 2);
        header = header.substr(header.find("\r\n") + 2);
    }
    //print map
    for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); it++)
    {
        std::cout << it->first << " => " << it->second << std::endl;
    }

    
}

std::string cgi::getResponse()
{
    return this->response;
}


void cgi::excuteCgi()
{
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return;
    }
    cpid = fork();
    if (cpid == -1)
    {
        perror("fork");
        return;
    }
    if (cpid == 0)
    { /* Child reads from pipe */
        close(pipefd[1]); /* Close unused write end */
        dup2(pipefd[0], STDIN_FILENO);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[0]);
        execve(argv[0], argv, envp);
        perror("execve");
        exit(EXIT_FAILURE);
    }
    else
    { /* Parent writes argv[1] to pipe */
        close(pipefd[0]); /* Close unused read end */
        write(pipefd[1], this->body.c_str(), this->body.size());
        close(pipefd[1]); /* Reader will see EOF */
        wait(NULL);       /* Wait for child */
        std::string tmp;
        while (read(pipefd[0], &buf, 1) > 0)
            tmp += buf;
        this->response = tmp;
    }
}