#include "../webserv/webserv.hpp"

int Accept(int socket, sockaddr *address,socklen_t  *address_len)
{
    int new_socket = accept(socket, address, address_len);
    if (new_socket < 0)
        perror("accept error");
    return new_socket;
}

int Listen(int socket, int backlog)
{
    int ret = listen(socket, backlog);
    if (ret < 0)
        perror("listen error");
    return ret;
}

int Bind(int socket,const struct sockaddr *address, socklen_t address_len)
{
    int ret = bind(socket, address, address_len);
    if (ret < 0)
        perror("bind error");
    return ret;
}

int Socket(int domain, int type, int protocol)
{
    int ret = socket(domain, type, protocol);
    if (ret < 0)
        perror("socket error");
    return ret;
}

size_t Send (int socket, const void *buffer, size_t length, int flags)
{
    size_t ret = send(socket, buffer, length, flags);
    if (ret < 0)
        perror("send error");
    return ret;
}


int Close(int fd)
{
    int ret = close(fd);
    if (ret < 0)
        perror("close error");
    return ret;
}
int Poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    int ret = poll(fds, nfds, timeout);
    if (ret < 0)
        perror("poll error");
    return ret;

}
int Setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len)
{
    int ret = setsockopt(socket, level, option_name, option_value, option_len);
    if (ret < 0)
        perror("setsockopt error");
    return ret;
}

int Fcntl(int fd, int cmd, int flags)
{
    int ret = fcntl(fd, cmd, flags);
    if (ret < 0)
        perror("fcntl error");
    return ret;
}


time_t getTime()
{
    return std::time(NULL);
}

bool isTimedOut(time_t start_time)
{
    if ( getTime() - start_time > TIMEOUT)
        return true;
    return false;
}

