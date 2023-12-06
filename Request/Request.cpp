#include "Request.hpp"
#include <sstream>

Request::Request()
{
    _isError = false;
    _isReadBody = false;
    _headerIsSend = false;
    // location = NULL;
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




std::string *Request::getHeader(std::string header)
{
    if(_headers.find(header) ==_headers.end())
        return NULL;
    return &this->_headers[header];
}

void Request::uriToPath()
{
    std::string tmp;
    std::stringstream ss;
    tmp = _headers["Uri"];
    if(tmp.find('?') != std::string::npos)
        tmp = tmp.substr(0, tmp.find('?'));
    size_t index = tmp.find_first_not_of('/');
    if(index != std::string::npos)
        tmp = tmp.substr(index - 1);
    for(size_t i = 0;i < tmp.length();i++)
    {
        if(tmp.at(i)== '%')
        {
            std::string tmp2;
            tmp2 = tmp.substr(i + 1, 2);
            ss <<std::hex << tmp2;
            tmp.replace(i, 3, ss.str());
        }
    }
    _headers["Path"] = tmp;
}

void Request::matchlocationForGET()
{
    
}
void Request::matchlocationForPOST()
{

}
void Request::matchlocationForDELETE()
{

}

void Request::parseBody()
{
    std::stringstream ss;
    std::string *tmp = NULL;
    // tmp = getHeader("Content-Length");
    // if(tmp)
    // {
    //     ss << tmp->c_str();
    //     ss >> _contentLength;
    //     if(_contentLength < _body.length())
    //         return (_errorCode = 413 ,void()); // Request Entity Too Large
    // }
    tmp = getHeader("Content-Type");
    if(tmp)
    {
        if(tmp->find("boundary=") != std::string::npos)
        {
            _isboundry = true;
            _boundry = tmp->substr(tmp->find("boundary=") + 9);
        }
    }
}
void Request::readBoundry()
{

}

void Request::readBoundrywithChunked()
{

}


void Request::readChunked()
{

}



