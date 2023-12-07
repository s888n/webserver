#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <fstream>
#include <map>
class location;
class server;
class Request
{
    protected:
        std::map<std::string, std::string> _headers;
        std::string _body;
        location    *_location;
        std::string _boundry;
        bool        _isError;
        int         _errorCode;
        bool        _headerIsRecv;
        bool        _isReadBody;
        bool        _isboundry;
        std::ofstream *_os;
        std::string _pathFile;
        std::string _pathDir;
        size_t      _contentLength;
        server      *_server;
    public:

        Request();
        void ParseRequest(std::string request);
        std::string *getHeader(std::string header);
        void checkRequest();
        bool checkUri();
        bool checkVirsion();
        bool checkMethod();
        void uriToPath();
        void matchlocation();

        void parseBody();
        void readBoundry();
        void readBoundrywithChunked(); 
        void readChunked();


        void findlocation();
        void tryFiles();

        //not implemented
        void matchlocationForGET();
        void matchlocationForPOST();
        void matchlocationForDELETE();

        bool getIsReadBody();
        int getErrorCode();
        bool getIsheaderIsRecv();
        ~Request();
};

#endif