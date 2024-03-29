#include "Request.hpp"
#include "../server/server.hpp"
#include <sstream>
#include <cstring>
#include <limits.h>

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
    _isCgi = false;
    _isNotRemove = false;
    _locationCgi = NULL;
    _cgi = NULL;
    _isNotFirst = false;
    isBodyEnd = false;

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
    if(_headers.find("Host") == _headers.end())
        throw (_isError = true , "host not found");
    if(_headers.find("Content-Length") != _headers.end())
    {
        std::stringstream ss;
        ss << _headers["Content-Length"];
        ss >> _contentLength;
    }
    if(_headers.find("Content-Type") != _headers.end() && _headers["Content-Type"].find("boundary=") != std::string::npos)
    {
        _isboundry = true;
        _boundry = _headers["Content-Type"].substr(_headers["Content-Type"].find("boundary=") + 9);
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
    std::string     pathreal,rootreal;
    char hold1[PATH_MAX];
    char hold2[PATH_MAX];
    std::string tmp;
    findlocation();
    tmp = _location->root + _headers["Path"];
    realpath(tmp.c_str(),hold1);
    realpath(_location->root.c_str(),hold2);
    pathreal = hold1;
    rootreal = hold2;
    if(pathreal.find(rootreal) != 0)
        throw (_errorCode = 400,_isError =true ,"method error");
      
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
    if(access(_pathFile.c_str(), R_OK) != 0)
        throw (_errorCode = 403,_isError =true ,"method error"); // Forbidden
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
    if(S_ISDIR(_stat.st_mode))
    {
        _pathDir = tmp;
        if(_headers["Path"].back() != '/')
        {
            _location->isReturn = true;
            _location->_return.first = 301;
            _location->_return.second = "http://" + _headers["Host"] + _headers["Path"] + "/";
            throw "return";
        }
        for(size_t i = 0; i < _location->indexes.size();i++)
        {
            std::string tmp2;
            if(tmp.back() == '/')
                tmp2 = tmp + _location->indexes[i];
            else
                tmp2 = tmp + "/" + _location->indexes[i];
            stat(tmp2.c_str(), &_stat);
            if(S_ISREG(_stat.st_mode))
            {
                _pathFile = tmp2;
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
                _mylocation = _server->locations[i];
                 _location = &_mylocation;
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
            throw (_errorCode = 413,_isError =true ,"method error"); // Length Required
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
    {
        if(access(tmp.c_str(), W_OK) != 0)
            throw (_errorCode = 403,_isError =true ,"method error");
        return (_pathFile = tmp,void());
    }
    else if(S_ISREG(_stat.st_mode))
        throw (_errorCode = 409,_isError =true ,"method error");
    else if(_headers.find("Content-Type") != _headers.end() && _headers["Content-Type"].find("boundary=") != std::string::npos)
        throw (_errorCode = 404 ,_isError =true ,"method error");
    else
    {
        stat(tmp.substr(0,tmp.find_last_of('/')).c_str(), &_stat);
        if(S_ISDIR(_stat.st_mode))
        {
            if(access(tmp.substr(0,tmp.find_last_of('/')).c_str(), W_OK) != 0)
                throw (_errorCode = 403,_isError =true ,"method error");
            return (_pathFile = tmp,void());
        }
        else
            throw (_errorCode = 404,_isError = true ,"method error");
    }
}
void Request::checkAccess(std::string path)
{
    DIR *dir;
    std::string tmp;
    struct dirent *en;
    struct stat _stat;
    dir  = opendir(path.c_str());
    if(dir == NULL)
        return;
    while((en = readdir(dir)))
    {
        tmp = path + en->d_name;
        stat(tmp.c_str(),&_stat);
        if(S_ISDIR(_stat.st_mode))
        {
            if(std::string(en->d_name) == "." || std::string(en->d_name) == "..")
                continue;
            if(tmp.back() != '/')
                tmp += '/';
            removeDir(tmp.c_str());
        }
        else if(S_ISREG(_stat.st_mode))
        {
            if(access(tmp.c_str(), W_OK) != 0)
                _isNotRemove = true;
        }
    }
    if(access(path.c_str(), W_OK) != 0)
        _isNotRemove = true;
    closedir(dir);
}

void Request::removeDir(std::string path)
{
    DIR *dir;
    std::string tmp;
    struct dirent *en;
    struct stat _stat;
    dir  = opendir(path.c_str());
    if(dir == NULL)
    {
        return;
    }
    while((en = readdir(dir)))
    {
        tmp = path + en->d_name;
        stat(tmp.c_str(),&_stat);
        if(S_ISDIR(_stat.st_mode))
        {
            if(std::string(en->d_name) == "." || std::string(en->d_name) == "..")
                continue;
            if(tmp.back() != '/')
                tmp += '/';
            removeDir(tmp.c_str());
            tmp.pop_back();
            remove(tmp.c_str());
        }
        else if(S_ISREG(_stat.st_mode))
            remove(tmp.c_str());
    }
    if(path.back() == '/')
        path.pop_back();
    remove(path.c_str());
    closedir(dir);
}

void Request::tryfileDelete()
{
    struct stat     _stat;
    std::string     tmp;

    tmp = _location->root;
    if(tmp.back() == '/')
        tmp = tmp.substr(0, tmp.length() - 1);
    tmp = tmp + _headers["Path"];
    stat(tmp.c_str(), &_stat);
    if(S_ISDIR(_stat.st_mode))
    {
        if(tmp.back() == '/')
        {
            checkAccess(tmp);
            if(_isNotRemove == true)
                throw (_errorCode = 403,_isError =true ,"method error");
            removeDir(tmp);
        }
        else
            throw (_errorCode = 409,_isError =true ,"method error");
    }else if(S_ISREG(_stat.st_mode))
            std::remove(tmp.c_str());
        else
            throw (_errorCode = 404,_isError =true ,"method error");
    throw (_errorCode = 204 ,"method error");
}

void Request::matchlocationForDELETE()
{
    if(_location->isReturn == true)
        throw "return";
    if(std::find(_location->methods.begin(), _location->methods.end(), "DELETE") == _location->methods.end())
        throw (_errorCode = 405,_isError =true ,"method error"); // Method Not Allowed
    tryfileDelete();
}

void Request::matchCgi()
{
    std::string tmp;
    std::string cgi_path;
    std::string path;
    struct stat     _stat;
    std::string filename;
    
    tmp = _headers["Path"];
    tmp = tmp.substr(tmp.find_last_of('.'));
    tmp = "*" + tmp;
    for(size_t i = 0; i < _server->locations.size(); i++)
    {
        if(_server->locations[i].path == tmp)
        {
            _isCgi = true;
            _locationCgi = &_server->locations[i];
            _location  = _locationCgi;
            tmp = _headers["Path"];
            filename = tmp.substr(tmp.find_last_of('/') + 1);
            if(tmp.find("cgi-bin/") == std::string::npos)
                _isCgi = false;
            else
                path = _locationCgi->cgi_path + "/"+filename;
            break ;
        }
    }
    if(_isCgi == false)
        return ;
    if(std::find(_locationCgi->methods.begin(), _locationCgi->methods.end(), _headers["Method"]) == _locationCgi->methods.end())
        return (_isCgi = false ,void());
    if(_headers["Method"] == "POST")
        _isReadBody = true;
    stat(path.c_str(), &_stat);
    if(S_ISREG(_stat.st_mode))
    {
        _pathFile = path;
        return ;
    }
    _isCgi = false;
}




