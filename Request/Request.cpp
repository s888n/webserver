#include "Request.hpp"


Request::Request()
{
}

Request::~Request()
{
}


void Request::ParseRequest(std::string request)
{
    std::string tmp;

    tmp = request.substr(0, request.find("\r\n"));
    this->_headers["Method"] = tmp.substr(0, tmp.find(" "));
    tmp = tmp.substr(tmp.find(" ") + 1);
    this->_headers["Uri"] = tmp.substr(0, tmp.find(" "));
    tmp = tmp.substr(tmp.find(" ") + 1);
    this->_headers["Version"] = tmp;
    request = request.substr(request.find("\r\n") + 2);
    while(request != "\r\n" && request != "")
    {
        _headers[request.substr(0,request.find(":"))] = request.substr(request.find(":") + 2, request.find("\r\n")-request.find(":") - 2);
        request = request.substr(request.find("\r\n") + 2);
    }
}

void Request::checkRequest()
{
    if(!checkVirsion())
       return(_isError = true, void());
    if(!checkUri())
        return(_isError = true, void());
    if(!checkMethod())
        return(_isError = true, void());
}

bool Request::checkUri()
{
    std::string validChar = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
    std::string tmp;
    tmp = _headers["uri"];
    if(tmp.length() > 2048)
        return (_errorCode = 414 ,false); // Request-URI Too Long
    if(tmp.front() != '/')
        return (_errorCode = 400 ,false); // Bad Request
    for(size_t i = 0;i < tmp.length();i++)
        if(!validChar.find(tmp.at(i)))
            return (_errorCode = 400 ,false); // Bad Request
    return true;
}

bool Request::checkVirsion()
{
    std::string tmp;
    std::string protocol = "HTTP/";
    tmp = _headers["Version"];
    if(protocol.compare(0, protocol.length(), tmp) != 0)
        return (_errorCode = 400 ,false); // Bad Request
    if(tmp != "HTTP/1.1")
        return (_errorCode = 505 ,false); // HTTP Version Not Supported
    return true;
}

bool Request::checkMethod()
{
    std::string tmp;
    tmp = _headers["Method"];
    if(tmp != "GET" && tmp != "POST" && tmp != "DELETE" )                                        
        return (_errorCode = 501 ,false);  // Not Implemented
    return true;
}

void Request::matchlocation()
{
    
}


std::string *Request::getHeader(std::string header)
{
    if(_headers.find(header) ==_headers.end())
        return NULL;
    return &this->_headers[header];
}   

