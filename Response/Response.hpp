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
        std::unordered_map<std::string, std::string> _headersResponse;
        std::map <std::string, std::string> _headersRequest;
        std::map<std::string, std::string> _MimeType;
        std::map<int, std::string> _errorPages;
        std::map<int,std::string> _status;
        std::fstream    fileSend;
        int             _statusCode;
        std::string     _header;
        bool            _isheadSend;
        bool           _isBodyEnd;
        bool            _isConnectionClose;
        std::string     _bodyResponse;

    public:
        std::string     _file;
        Response();
        Response(Response const &main);
        Response& operator=(Response const &main);
        void makeHeader(int status);
        void sendHeaders(int fd);
        void sendBody(int fd);
        void sendBodyString(int fd);
        void makeBody();
        void sendRangeBody(int fd,size_t start);
        bool getIsheadSend() const;
        void sendExaption(int fd, int status);
        void fillResponseMap();
        void createLengthHeader();
        bool getIsBodyEnd() const;
        bool getIsConnectionClose() const;
        ~Response();
};

#endif