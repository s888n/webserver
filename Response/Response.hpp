#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <unordered_map>
#include <map>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
class Response
{
    protected:
        std::unordered_map<std::string, std::string> _headers;
        std::map<int,std::string> _status;
        std::string _filePath;
        std::fstream *fileSend;
        std::string _header;
        bool _isheadSend;

    public:
        Response();
        void makeHeader(int status);
        void sendHeaders(int fd);
        void sendBody(int fd);
        void sendRangeBody(int fd,size_t start);
        ~Response();
};

#endif