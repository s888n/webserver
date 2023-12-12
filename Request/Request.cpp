#include "Request.hpp"
#include "../server/server.hpp"
#include <sstream>

Request::Request()
{
    _isError = false;
    _isReadBody = false;
    _headerIsRecv = false;
    _isboundry = false;
    _errorCode = 0;
    _contentLength = 0;
    _location = NULL;
    _server = NULL;
    _os = NULL;

    // location = NULL;
}

Request::~Request()
{

}

int Request::getErrorCode()
{
    return _errorCode;
}


bool Request::getIsReadBody()
{
    return _isReadBody;
}

bool Request::getIsheaderIsRecv()
{
    return _headerIsRecv;
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
        throw (_isError = true , "http version not supported");
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
    findlocation();
    if(_headers["Method"] == "GET")
        matchlocationForGET();
    else if(_headers["Method"] == "POST")
        matchlocationForPOST();
    else if(_headers["Method"] == "DELETE")
        matchlocationForDELETE();
}

void Request::matchlocationForGET()
{
    _isReadBody = false;
    if(_location->isReturn == true)
        throw "return";
    if(std::find(_location->methods.begin(), _location->methods.end(), "GET") == _location->methods.end())
        throw (_errorCode = 405,_isError =true ,"method error"); // Method Not Allowed
    tryFiles();
}

void Request::tryFiles()
{
    std::string     tmp;
    struct stat     _stat;
    std::fstream    fs;

    tmp = _location->root;
    if(tmp.back() == '/')
        tmp = tmp.substr(0, tmp.length() - 1);
    tmp = tmp + _headers["Path"];
    stat(tmp.c_str(), &_stat);
    fs.open(tmp.c_str(), std::fstream::in);
    if(S_ISDIR(_stat.st_mode))
    {
        _pathDir = tmp;
        for(size_t i = 0; i < _location->indexes.size();i++)
        {
            std::string tmp2;
            if(tmp.back() == '/')
                tmp2 = tmp + _location->indexes[i];
            else
                tmp2 = tmp + "/" + _location->indexes[i];
            fs.open(tmp2.c_str(), std::fstream::in);
            if(S_ISREG(_stat.st_mode))
            {
                _pathFile = tmp2;
                _location->autoindex = false;
                return ;
            }
        }
        if (_location->autoindex == true)
            throw (_errorCode = 0,_pathFile = tmp, "autoindex");
        else
            throw (_errorCode = 403,_isError =true ,"method error");
    }
    else if(S_ISREG(_stat.st_mode))
    {
        _pathFile = tmp;
        return;
    }
    throw (_errorCode = 404,_isError =true ,"method error");
}

void Request::findlocation()
{
     std::string tmp;
    tmp = _headers["Path"];
    if(tmp.back() != '/')
        tmp += "/";
    _location = NULL;
    while(_location == NULL)
    {
        for(size_t i = 0; i < _server->locations.size(); i++)
        {
            if(tmp == _server->locations[i].path)
            {
                _location = &_server->locations[i];
                break ;
            }
        }
        if(tmp == "/")
            break;
        if(tmp.back() == '/')
            tmp = tmp.substr(0, tmp.length() - 1);
        else
            tmp = tmp.substr(0, tmp.find_last_of('/') + 1);
    }
}

void Request::matchlocationForPOST()
{
    std::stringstream ss;
    size_t tmp;
    if(_location->isReturn == true)
        throw "return";
    if(std::find(_location->methods.begin(), _location->methods.end(), "POST") == _location->methods.end())
        throw (_errorCode = 405,_isError =true ,"method error"); // Method Not Allowed
    if(_headers.find("Content-Length") != _headers.end())
    {
        ss << _headers["Content-Length"];
        ss >> tmp;
        if(tmp > _location->max_body_size)
            throw (_errorCode = 411,_isError =true ,"method error"); // Length Required
    }
    tryfilePost();
    _isReadBody = true;
}


void Request::tryfilePost()
{
    struct stat     _stat;
    std::string     tmp;

    tmp = _location->root;
    if(tmp.back() == '/')
        tmp = tmp.substr(0, tmp.length() - 1);
    tmp = tmp + _headers["Path"];
    stat(tmp.c_str(), &_stat);
    if(S_ISDIR(_stat.st_mode))
        return (_pathFile = tmp,void());
    else if(S_ISREG(_stat.st_mode))
        throw (_errorCode = 409,_isError =true ,"method error");
    else
    {
        stat(tmp.substr(0,tmp.find_last_of('/')).c_str(), &_stat);
        if(S_ISDIR(_stat.st_mode))
            return (_pathFile = tmp,void());
        else
            throw (_errorCode = 404,_isError =true ,"method error");
    }
}

void Request::matchlocationForDELETE()
{

}

void Request::parseBody()
{
    // std::stringstream ss;
    // std::string *tmp = NULL;
    // // tmp = getHeader("Content-Length");
    // // if(tmp)
    // // {
    // //     ss << tmp->c_str();
    // //     ss >> _contentLength;
    // //     if(_contentLength < _body.length())
    // //         return (_errorCode = 413 ,void()); // Request Entity Too Large
    // // }
    // tmp = getHeader("Content-Type");
    // if(tmp)
    // {
    //     if(tmp->find("boundary=") != std::string::npos)
    //     {
    //         _isboundry = true;
    //         _boundry = tmp->substr(tmp->find("boundary=") + 9);
    //     }
    // }
}

void Request::readBoundry(int fd)
{
    char buffer[3040];
    int ret = 1;
    ret = recv(fd, buffer, 3040, 0);
    if(ret > 0)
    {
        timestamp = time(NULL);
        _body.append(buffer, ret);
        _boundry = _headers["Content-Type"].substr(_headers["Content-Type"].find("boundary=") + 9);
        if(_body.find("--"+_boundry +"--") != std::string::npos)
        {
            std::cout << "end" << std::endl;
            isBodyEnd = true;
        }
    }

}

void Request::readBoundrywithChunked()
{

}


void Request::readChunked(int fd)
{
    char buffer[3040];
    int ret = 1;
    ret = recv(fd, buffer, 3040, 0);
    if(ret > 0)
    {
    timestamp = time(NULL);
    _body.append(buffer, ret);
    if(_body.find("0\r\n\r\n") != std::string::npos)
        isBodyEnd = true;
    }
}
void  Request::readContentLength(int fd)
{
    char buffer[3040];
    int ret = 1;
    ret = recv(fd, buffer, 3040, 0);
    if(ret > 0)
    {
        timestamp = time(NULL);
        _body.append(buffer, ret);
        if(_body.length() == _contentLength)
            isBodyEnd = true;
        else if(_body.length() > _contentLength || _contentLength > _location->max_body_size)
            throw (_errorCode = 413 ,"return"); // Request Entity Too Large

    }
}



