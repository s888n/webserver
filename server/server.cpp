#include "server.hpp"

void server::setPort(int port)
{
    this->port = port;
}

void server::setSocket(int fd)
{
    this->fd = fd;
}

void server::setAutoindex(bool autoindex)
{
    this->autoindex = autoindex;
}

void server::setMaxBodySize(size_t max_body_size)
{
    this->max_body_size = max_body_size;
}

void server::setAddr(struct sockaddr_in addr)
{
    this->addr = addr;
}

void server::setHost(std::string host)
{
    this->host = host;
}

void server::setRoot(std::string root)
{
    this->root = root;
}

void server::setIndexes(std::vector<std::string> indexes)
{
    this->indexes = indexes;
}

void server::setMethods(std::vector<std::string> methods)
{
    this->methods = methods;
}

void server::setServerNames(std::vector<std::string> server_names)
{
    this->server_names = server_names;
}

void server::setErrorPages(std::map <int, std::string> error_pages)
{
    this->error_pages = error_pages;
}

int server::getPort() const
{
    return this->port;
}

int server::getSocket() const
{
    return this->fd;
}

bool server::getAutoindex() const
{
    return this->autoindex;
}

size_t server::getMaxBodySize() const
{
    return this->max_body_size;
}

struct sockaddr_in server::getAddr() const
{
    return this->addr;
}

std::string server::getHost() const
{
    return this->host;
}

std::string server::getRoot() const
{
    return this->root;
}

std::vector<std::string> server::getIndexes() const
{
    return this->indexes;
}

std::vector<std::string> server::getMethods() const
{
    return this->methods;
}

std::vector<std::string> server::getServerNames() const
{
    return this->server_names;
}


std::map <int, std::string> server::getErrorPages() const
{
    return this->error_pages;
}

void server::setLocations(std::vector<location> locations) 
{
    this->locations = locations;
}

std::vector<location> server::getLocations() const
{
    return this->locations;
}

void server::createSocket(void)
{
    int opt = 1;

    fd = Socket(AF_INET, SOCK_STREAM, 0);

    Setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    Bind(fd, (struct sockaddr *)&addr, sizeof(addr));

    Listen(fd, SOMAXCONN);

}