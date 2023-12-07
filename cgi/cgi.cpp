#include "cgi.hpp"



std::string cgi::execute(std::string &compiler, std::string &scriptPath)
{
    pipe_fd = new int[2];
    argv = new char*[3];
    env = new char*[1];

    argv[0] = new char[compiler.size() + 1];
    argv[1] = new char[scriptPath.size() + 1];
    argv[2] = NULL;
    strcpy(argv[0], compiler.c_str());
    strcpy(argv[1], scriptPath.c_str());
    env[0] = NULL;
    if (pipe(pipe_fd) < 0)
        perror("pipe error");
    pid = fork();
    if (pid < 0)
        perror("fork error");
    
    if (pid == 0)
    {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], 1);
        close(pipe_fd[1]);
        execve(argv[0], argv, env);
        exit(0);
    }
    else
    {
        close(pipe_fd[1]);
        char buf[1024];
        std::string ret;
        while (1)
        {
            int n = read(pipe_fd[0], buf, 1024);
            if (n == 0)
                break;
            buf[n] = '\0';
            ret += buf;
        }
        close(pipe_fd[0]);
        waitpid(pid, &status, 0);
        delete[] argv[0];
        delete[] argv[1];
        delete[] argv;
        delete[] env;
        delete[] pipe_fd;
        return ret;
    }
}