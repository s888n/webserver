#include "Response.hpp"
#include <sys/stat.h>

Response::Response()
{
    std::cout << "Response constructor" << std::endl;
    _status[200] = "OK";
    _status[201] = "Created";
    _status[202] = "Accepted";
    _status[204] = "No Content";
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
    _isheadSend = false;
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

    _headersResponse["Content-Length"] = std::to_string(filestat.st_size);
    makeHeader(200);
    std::cout << _header << std::endl;
    ret = send(fd,_header.c_str(),_header.length(),0);
    if(ret <= 0)
        return;
    _isheadSend = true;
    fileSend.open(_file, std::ios::in);
}

void Response::sendBody(int fd)
{
    char *buffer = new char[1024];
    int ret;
    fileSend.read(buffer, 1024);
    ret = fileSend.gcount();
    if(ret <=  0)
    {
        delete[] buffer;
        fileSend.close();
        return;
    }
    ret = send(fd,buffer,ret,0);
    if(ret <= 0)
    {
        delete[] buffer;
        fileSend.close();
        return;
    }
}

bool Response::getIsheadSend() const
{
    return _isheadSend;
}

void Response::sendRangeBody(int fd ,size_t start)
{
    char *buffer = new char[102400];
    int ret;
    fileSend.seekg(start);
    fileSend.read(buffer, 102400);
    ret = fileSend.gcount();
    if(ret <=  0)
    {
        delete[] buffer;
        fileSend.close();
        return;
    }
    _header = "HTTP/1.1 206 Partial Content\r\n";
    _header += "Content-Range: bytes " + std::to_string(start) + "-" + std::to_string(start + ret) + "/" + std::to_string(fileSend.tellg()) + "\r\n";
    _header += "Content-Length: " + std::to_string(ret) + "\r\n";
    _header += "Content-Type: " + _headersResponse["Content-Type"] + "\r\n";
    _header += "Accept-Ranges: bytes\r\n";
    _header += "Connection: keep-alive\r\n"; 
    _header += "\r\n";
    _header.append(buffer, ret);
    ret = send(fd,_header.c_str(),_header.size(),0);
    if(ret <= 0)
    {
        delete[] buffer;
        fileSend.close();
        return;
    }
    delete[] buffer;
    fileSend.close();
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
    }
    return *this;
}


void Response::fillResponseMap()
{
    // std::cout << "extension : " << _file.substr(_file.find_last_of('.')) << std::endl;
    //print MimeType
    if(_MimeType[_file.substr(_file.find_last_of('.'))].length() > 0)
        _headersResponse["Content-Type"] = _MimeType[_file.substr(_file.find_last_of('.'))];
    else
         _headersResponse["Content-Type"] = "application/octet-stream";  
    _headersResponse["Server"] = "Webserve";
    _headersResponse["Host"] = _headersRequest["Host"];

    _headersResponse["Accept-Ranges"] = "bytes";
    _headersResponse["Connection"] = "Keep-Alive";

}
void  Response::sendExaption(int fd, int status)
{
    (void) fd;
    (void) status;
}


