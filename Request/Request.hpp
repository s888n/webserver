#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <fstream>
#include <map>
#include "../server/location.hpp"
// class location;
class server;
class cgi;
class Request
{
    protected:
        std::map<std::string, std::string> _headers;
        std::string _boundry;
        bool        _headerIsRecv;
        bool        _isReadBody;
        int         _errorCode;
        bool        _isboundry;
        std::ofstream *_os;
        std::string _pathDir;
        size_t      _contentLength;
        location    *_locationCgi;
        bool       _isNotFirst;
        location    _mylocation;
        
    public:

        bool        _isCgi;
        cgi         *_cgi;
        location    *_location;
        bool        _isError;
        std::string _pathFile;
        server      *_server;
        std::string _body;
        bool _isNotRemove;
        bool isBodyEnd;
        
        Request();
        time_t timestamp;
        void ParseRequest(std::string request);
        std::string *getHeader(std::string header);
        void checkRequest();
        bool checkUri();
        bool checkVirsion();
        bool checkMethod();
        void uriToPath();
        void matchlocation();

        void parseBody();
        void readBoundrywithChunked(); 


        void findlocation();
        void tryFiles();

        //not implemented
        void matchlocationForGET();
        void matchlocationForPOST();
        void matchlocationForDELETE();

        bool getIsReadBody();
        int getErrorCode();
        ~Request();
        void tryfilePost();
        void tryfileDelete();
        void removeDir(std::string path);
        void checkAccess(std::string path);
        void matchCgi();
};

#endif