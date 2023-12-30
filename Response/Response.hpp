#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <unordered_map>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <sys/socket.h>
#include <utility>

class location;
class Response
{
    protected:
        std::map<std::string, std::string> _headersResponse;
        std::map<std::string, std::string> _MimeType;
        std::map<int, std::string> _errorPages;
        std::map<int,std::string> _status;
        std::fstream    fileSend;
        int             _statusCode;
        std::string     _header;
        bool            _isheadSend;
        bool            _isConnectionClose;
        std::string     _servername;
        size_t          _contentLengthSend;
        std::vector<std::pair<std::string, std::string> > _headersCgi;

    public:
        bool _isBodyEnd;
        bool isBodyString;
        std::map <std::string, std::string> _headersRequest;
        location        *_locationResponse;
        std::string     _bodyResponse;
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
        void sendExaption(int fd, int status);
        void fillResponseMap();
        void createLengthHeader();
        ~Response();
};

#endif