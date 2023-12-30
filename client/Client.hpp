#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <iostream>
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "../server/location.hpp"
#include "../server/server.hpp"
#include "../cgi/cgi.hpp"
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// class Request;
// class Response;

class Client : public Request, public Response
{
    private:
        std::string _request;
        bool _isparsed;
        std::string _host;
        bool _toParseBody;
        std::string _fileupload;
        size_t _chunkedSize;
        size_t _currentLength;
        std::vector<std::string> _filesUploaded;
        // int _port;
    public:
        std::vector<server> *_servers;
        
        int _socket;
        struct sockaddr_in addr;
        socklen_t addrlen;
        int serverFd;
        bool isBodyString;
        std::string filename;
        std::string boundary;
        std::string _bodyHyprid;
        bool _isreadFromClient;
        Client();
        void readRequest();
        void readheader();
        server& findServer();
        void readbody();
        void sendResponse();
        bool getIsParsed() const;
        void fillResponseMap();
        bool checkReturn();
        void parseRequestBody();
        void createFile(std::string path);
        void createFile(std::string path,std::string &fileContent);
        void parseMultipartData();
        void parseBinaryData();
        void parseChunkedData();
        std::string unchunk(std::string &chunked);
        std::string generateRandomString();
        std::string getFileName(std::string fileHeader);
        size_t fileCount();
        ~Client();
        void parseHyprid();
        void checkIsBodyEnd();
        void  readContentLength(int fd);
        void readChunked(int fd);
        void readBoundry(int fd);
        void readHyprid(int fd);
         void readChunkedCgi(int fd);
        void readBoundryCgi(int fd);
        void readContentLengthCgi(int fd);
};

#endif