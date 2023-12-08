#include "Response.hpp"
#include <sys/stat.h>
#include <sstream>

Response::Response()
{
    _status[200] = "OK";
    _status[201] = "Created";
    _status[202] = "Accepted";
    _status[204] = "No Content";
    _status[206] = "Partial Content";
    _status[301] = "Moved Permanently";
    _status[302] = "Found";
    _status[304] = "Not Modified";
    _status[400] = "Bad Request";
    _status[401] = "Unauthorized";
    _status[403] = "Forbidden";
    _status[404] = "Not Found";
    _status[405] = "Method Not Allowed";
    _status[413] = "Payload Too Large";
    _status[414] = "URI Too Long";
    _status[500] = "Internal Server Error";
    _status[501] = "Not Implemented";
    _status[505] = "HTTP Version Not Supported";

    _MimeType[".html"] = "text/html";
    _MimeType[".css"] = "text/css";
    _MimeType[".js"] = "text/javascript";
    _MimeType[".jpg"] = "image/jpeg";
    _MimeType[".jpeg"] = "image/jpeg";
    _MimeType[".png"] = "image/png";
    _MimeType[".gif"] = "image/gif";
    _MimeType[".swf"] = "application/x-shockwave-flash";
    _MimeType[".txt"] = "text/plain";
    _MimeType[".mp3"] = "audio/mpeg";
    _MimeType[".mp4"] = "video/mp4";
    _MimeType[".avi"] = "video/x-msvideo";
    _MimeType[".mpeg"] = "video/mpeg";
    _MimeType[".pdf"] = "application/pdf";
    _MimeType[".zip"] = "application/zip";
    _MimeType[".rar"] = "application/x-rar-compressed";
    _MimeType[".gz"] = "application/gzip";
    _MimeType[".tar"] = "application/x-tar";
    _MimeType[".json"] = "application/json";
    _MimeType[".xml"] = "application/xml";
    _MimeType[".bin"] = "application/octet-stream";
    _MimeType[".exe"] = "application/octet-stream";
    _MimeType[".dll"] = "application/octet-stream";
    _MimeType[".class"] = "application/octet-stream";
    _MimeType[".doc"] = "application/msword";

    _errorPages[400] = "error_pages/400.html";
    _errorPages[401] = "error_pages/401.html";
    _errorPages[403] = "error_pages/403.html";
    _errorPages[404] = "error_pages/404.html";
    _errorPages[405] = "error_pages/405.html";
    _errorPages[413] = "error_pages/413.html";
    _errorPages[414] = "error_pages/414.html";
    _errorPages[500] = "error_pages/500.html";
    _errorPages[501] = "error_pages/501.html";
    _errorPages[505] = "error_pages/505.html";

    _isheadSend = false;
    _isBodyEnd = false;
    _isConnectionClose = false;
}

void Response::makeHeader(int status)
{
    _header = "HTTP/1.1 " + std::to_string(status) + " " + _status[status] + "\r\n";
    while(_headersResponse.size() > 0)
    {
        _header += _headersResponse.begin()->first + ": " + _headersResponse.begin()->second + "\r\n";
        _headersResponse.erase(_headersResponse.begin());
    }
    _header += "\r\n";
}

void Response::sendHeaders(int fd)
{
    int ret;
    struct stat filestat;
    stat(_file.c_str(), &filestat);
    fillResponseMap();
    fileSend.open(_file, std::ios::in);
    if(fileSend.is_open())
        createLengthHeader();
    makeHeader(_statusCode);
    // std::cout << _header << std::endl;
    ret = send(fd,_header.c_str(),_header.size(),0);
    if(ret <= 0)
        return;
    _isheadSend = true;
    if(!fileSend.is_open())
    {
        _isBodyEnd = true;
        fileSend.close();
        return;
    }
}

void Response::sendBody(int fd)
{
    char *buffer = new char[10240];
    int ret;
    fileSend.read(buffer, 3600);
    ret = fileSend.gcount();
    if(ret <=  0)
    {
        //std::cout << "ret1 : " << ret << std::endl;
        delete[] buffer;
        fileSend.close();
        
        _isBodyEnd = true;
        return;
    }
    ret = send(fd,buffer,ret,0);
    if(ret <= 0)
    {
         std::cout << "ret2 : " << ret << std::endl;
        delete[] buffer;
        fileSend.close();
        _isBodyEnd = true;
        _isConnectionClose = true;
        return;
    }
}

bool Response::getIsheadSend() const
{
    return _isheadSend;
}

void Response::sendRangeBody(int fd ,size_t start)
{
    char *buffer = new char[10240];
    int ret;
    struct stat filestat;
    stat(_file.c_str(), &filestat);
    fileSend.open(_file, std::ios::in);
    if(!fileSend.is_open())
    {
        delete[] buffer;
        _isBodyEnd = true;
        return;
    }
    fileSend.seekg(start);
    fileSend.read(buffer, 10240);
    ret = fileSend.gcount();
    if(ret <=  0)
    {
        delete[] buffer;
        fileSend.close();
        return;
    }
    _header = "HTTP/1.1 206 Partial Content\r\n";
    _header += "Content-Range: bytes " + std::to_string(start) + "-" + std::to_string(start + ret) + "/" + std::to_string(filestat.st_size) + "\r\n";
    _header += "Content-Length: " + std::to_string(ret) + "\r\n";
    _header += "Content-Type: video/mp4\r\n";
    _header += "Accept-Ranges: bytes\r\n";
    _header += "Connection: keep-alive\r\n"; 
    _header += "\r\n";
    _header.append(buffer, ret);
    ret = send(fd,_header.c_str(),_header.size(),0);
    if(ret <= 0)
    {
        delete[] buffer;
        _isBodyEnd = true;
        fileSend.close();
        return;
    }
    delete[] buffer;
    fileSend.close();
    _isBodyEnd = true;
}
 Response::~Response()
 {
     if(fileSend.is_open())
        fileSend.close();
 }

Response::Response(Response const &main)
{
    if(this != &main)
        *this = main;
}


Response& Response::operator=(Response const &main)
{
    if(this != &main)
    {
        _headersResponse = main._headersResponse;
        _status = main._status;
        _file = main._file;
        _header = main._header;
        _isheadSend = main._isheadSend;
        _MimeType = main._MimeType;
        _headersRequest = main._headersRequest;
        _statusCode = main._statusCode;
        _errorPages = main._errorPages;
        _isBodyEnd = main._isBodyEnd;
        _isConnectionClose = main._isConnectionClose;
    }
    return *this;
}


void Response::fillResponseMap()
{
    // std::cout << "extension : " << _file.substr(_file.find_last_of('.')) << std::endl;
    //print MimeType
    _headersResponse["Content-Type"] = "application/octet-stream";  
    if(_file.find_last_of('.') != std::string::npos)
    {
    if(_MimeType.find(_file.substr(_file.find_last_of('.'))) != _MimeType.end())
        _headersResponse["Content-Type"] = _MimeType[_file.substr(_file.find_last_of('.'))];
    else
         _headersResponse["Content-Type"] = "application/octet-stream";  
    }
    _headersResponse["Server"] = "Webserve";
    // _headersResponse["Host"] = _headersRequest["Host"];

    _headersResponse["Accept-Ranges"] = "bytes";
    _headersResponse["Connection"] = "Keep-Alive";

}
void  Response::sendExaption(int fd, int status)
{
    (void) fd;
    (void) status;
}

bool Response::getIsBodyEnd() const
{
    return _isBodyEnd;
}

void Response::createLengthHeader()
{
    std::stringstream ss;
    size_t pos = 0;
    struct stat filestat;
    stat(_file.c_str(), &filestat);

    if(_headersRequest.find("Range") != _headersRequest.end())
    {
        std::string tmp = _headersRequest["Range"];
        tmp = tmp.substr(tmp.find("=") + 1);
        ss << tmp;
        ss >> pos;
        _headersResponse["Content-Length"] = std::to_string(filestat.st_size - pos);
        fileSend.seekg(pos);
        _headersResponse["Content-Range"] = "bytes " + std::to_string(pos) + "-" + std::to_string(filestat.st_size - 1) + "/" + std::to_string(filestat.st_size);
        _headersResponse["Accept-Ranges"] = "bytes";
        _statusCode = 206;

    }else
        _headersResponse["Content-Length"] = std::to_string(filestat.st_size);
}


bool Response::getIsConnectionClose() const
{
    return _isConnectionClose;
}


