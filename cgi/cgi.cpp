#include "cgi.hpp"

cgi::cgi(std::string _compiler, std::string _scriptPath, std::string _request)
{
    if (_compiler.size() == 0 || _scriptPath.size() == 0 || _request.size() == 0)
        throw std::invalid_argument("CGI : invalid arguments");
    if (scriptPath.find(".py") == std::string::npos && scriptPath.find(".js") == std::string::npos)
        throw std::invalid_argument("CGI : invalid script path");
    this->compiler = _compiler;
    this->request = _request;
    this->body = _request.substr(_request.find("\r\n\r\n") + 4);
    this->scriptPath = _scriptPath;
    parseEnv(request);
    argv = new char *[3];
    excuteCgi();
}
cgi::~cgi()
{
    for (int i = 0; envp[i] != NULL; i++)
        delete[] envp[i];
    delete[] envp;
    delete[] argv;
}

void cgi::parseEnv(std::string request)
{

    std::map<std::string, std::string> env;

    header = request.substr(0, request.find("\r\n\r\n") + 2);
    body = request.substr(request.find("\r\n\r\n") + 4);
    std::string requestLine = header.substr(0, header.find("\r\n"));
    header.erase(0, header.find("\r\n") + 2);

    std::string method = requestLine.substr(0, requestLine.find(" "));
    requestLine.erase(0, requestLine.find(" ") + 1);
    std::string path = requestLine.substr(0, requestLine.find(" "));
    requestLine.erase(0, requestLine.find(" ") + 1);
    std::string httpVersion = requestLine.substr(0, requestLine.find("\r\n"));

    env["REQUEST_METHOD"] = method;
    env["REQUEST_URI"] = path;
    env["SERVER_PROTOCOL"] = httpVersion;
    env["SERVER_SOFTWARE"] = "webserv";
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    env["CONTENT_LENGTH"] = std::to_string(body.size());
    if (body.size() > 0)
        env["QUERY_STRING"] = body;
    std::map<std::string, std::string> headers = getHeaders(header);
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
    {
        std::string key = "HTTP_" + it->first;
        std::string value = it->second;
        for (size_t i = 0; i < key.size(); i++)
            key[i] = toupper(key[i]);
        env[key] = value;
    }
    envp = mapToEnvp(env);
}

void cgi::excuteCgi()
{
    // return the result of the script in response
    int pipefd[2];
    pid_t cpid;
    char buf;
    argv[0] = new char[compiler.size() + 1];
    bzero(argv[0], compiler.size() + 1);
    strcpy(argv[0], compiler.c_str());
    argv[1] = new char[scriptPath.size() + 1];
    bzero(argv[1], scriptPath.size() + 1);
    strcpy(argv[1], scriptPath.c_str());
    argv[2] = NULL;

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
    {                     /* Child reads from pipe */
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
    {                     /* Parent writes argv[1] to pipe */
        close(pipefd[0]); /* Close unused read end */
        write(pipefd[1], this->body.c_str(), this->body.size());
        close(pipefd[1]); /* Reader will see EOF */
        wait(NULL);       /* Wait for child */
        std::string tmp;
        while (read(pipefd[0], &buf, 1) > 0)
            tmp += buf;
        this->response = tmp;
        delete[] argv[0];
        delete[] argv[1];
    }
}

std::string cgi::getResponse() { return this->response; }

char **mapToEnvp(std::map<std::string, std::string> &env)
{
    char **envp = new char *[env.size() + 1];
    int i = 0;
    for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); it++)
    {
        std::string tmp = it->first + "=" + it->second;
        envp[i] = new char[tmp.size() + 1];
        bzero(envp[i], tmp.size() + 1);
        strcpy(envp[i], tmp.c_str());
        i++;
    }
    envp[env.size()] = NULL;
    return envp;
}

std::map<std::string, std::string> getHeaders(std::string headers)
{
    std::map<std::string, std::string> _headers;
    std::string _key;
    size_t _headerkeyPos;
    size_t _headerValuePos;
    size_t _hpos = 0;
    size_t headersLength = headers.size();
    while (_hpos < headersLength)
    {
        _headerkeyPos = headers.find(':', _hpos);
        if (_headerkeyPos == std::string::npos)
            break;
        _key = headers.substr(_hpos, _headerkeyPos - _hpos);
        if ((_headerValuePos = headers.find("\r\n", _headerkeyPos)) == std::string::npos)
            return _headers;
        _headers[_key] = headers.substr(_headerkeyPos + 2, _headerValuePos - _headerkeyPos - 2);
        _hpos = _headerValuePos + 2;
    }
    return _headers;
}
