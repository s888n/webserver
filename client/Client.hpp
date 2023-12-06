#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <iostream>
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
class Request;
class Response;

class Client : public Request, public Response
{
    private:
        std::string _request;
        bool _isparsed;
        std::string _host;
        // int _port;
    public:

        int _socket;
        struct sockaddr_in addr;
        socklen_t addrlen;
        time_t timestamp;

        Client();
        void readRequest();
        void readheader();
        void readbody();
        void sendResponse();
        ~Client();
};

#endif