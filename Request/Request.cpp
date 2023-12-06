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

int Request::getErrorCode()
{
    return _errorCode;
}

void Request::ParseRequest(std::string request)
{
    std::string tmp;

    tmp = request.substr(0, request.find("\r\n"));
    this->_headers["Method"] = tmp.substr(0, tmp.find(" "));
    tmp = tmp.substr(tmp.find(" ") + 1);
    this->_headers["Version"] = tmp.substr(tmp.find_last_of(" ") + 1);
    tmp = tmp.substr(0,tmp.find_last_of(" "));
    this->_headers["Uri"]= tmp;
    request = request.substr(request.find("\r\n") + 2);
    while(request != "\r\n" && request != "")
    {
        _headers[request.substr(0,request.find(":"))] = request.substr(request.find(":") + 2, request.find("\r\n")-request.find(":") - 2);
        request = request.substr(request.find("\r\n") + 2);
    }
}

void Request::checkRequest()
{
    if(checkVirsion() == false)
        throw (_isError = true, "http version not supported");
    if(checkUri()== false)
        throw (_isError = true, "uri error");
    if(checkMethod()== false)
        throw (_isError = true, "method error");
}

bool Request::checkUri()
{
    std::string validChar = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
    std::string tmp;
    tmp = _headers["Uri"];
    std::cout << tmp << std::endl;
    if(tmp.length() > 2048)
        return (_errorCode = 414 ,false); // Request-URI Too Long
    if(tmp.front() != '/')
        return (_errorCode = 400 ,false);// Bad Request
    for(size_t i = 0;i < tmp.length();i++)
        if(validChar.find(tmp.at(i)) == std::string::npos)
            return (_errorCode = 400 ,false); // Bad Request
    return true;
}

bool Request::checkVirsion()
{
    std::string tmp;
    std::string protocol = "HTTP/";
    tmp = _headers["Version"];
    if(protocol != tmp.substr(0,5))
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
    if(tmp == "POST")
    {
        if(_headers.find("Content-Length") == _headers.end()  && _headers.find("Transfer-Encoding") == _headers.end())
            return (_errorCode = 411 ,false); // Length Required
        if(_headers.find("Transfer-Encoding") != _headers.end())
            if(_headers["Transfer-Encoding"] != "chunked")
                return (_errorCode = 501 ,false); // Not Implemented
    }
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
            int ch;
            tmp2 = tmp.substr(i + 1, 2);
            ss << std::hex << tmp2;
            ss >> ch;
            tmp.replace(i, 3, 1,ch);
        }
    }
    _headers["Path"] = tmp;
}


void Request::matchlocation()
{
    if(_headers["Method"] == "GET")
        matchlocationForGET();
    else if(_headers["Method"] == "POST")
        matchlocationForPOST();
    else if(_headers["Method"] == "DELETE")
        matchlocationForDELETE();
}

void Request::matchlocationForGET()
{
    _
    
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



