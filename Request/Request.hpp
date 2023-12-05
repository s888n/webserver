#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
class Request
{
    private:
        std::map<std::string, std::string> _headers;
        std::string _body;
        bool        _isError;
        int         _errorCode;
    public:
        Request();
        void ParseRequest(std::string request);
        std::string *getHeader(std::string header);
        void checkRequest();
        bool checkUri();
        bool checkVirsion();
        bool checkMethod();
        void matchlocation();
        ~Request();
};

#endif