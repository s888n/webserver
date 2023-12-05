#ifndef UTILS_HPP
#define UTILS_HPP

#include "../webserv/webserv.hpp"


int Accept(int socket, sockaddr *address,socklen_t  *address_len);
int Listen(int socket, int backlog);
int Bind(int socket,const struct sockaddr *address, socklen_t address_len);
int Socket(int domain, int type, int protocol);
size_t Send (int socket, const void *buffer, size_t length, int flags);
size_t Recv (int socket, void *buffer, size_t length, int flags);
int Close(int fd);
int Poll(struct pollfd *fds, nfds_t nfds, int timeout);
int Fcntl(int fd, int cmd, int arg);
int Setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
time_t getTime();
bool isTimedOut(time_t start_time);

#endif