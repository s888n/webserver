#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <iostream>
#include "Request/Request.hpp"
#include "Response/Response.hpp"
#include <fstream>
#include <sstream>
#include <sys/socket.h>

class Request;
class Response;

class Client : public Request, public Response
{
    private:
        std::string _request;
        bool _isparsed;
        std::string _host;
        int _port;
        int _socket;
    public:
        Client();
        void readRequest();
        void readheader();
        void readbody();
        void sendResponse();
        ~Client();
};

#endif