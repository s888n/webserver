#include "Response.hpp"

Response::Response()
{
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
    fileSend = NULL;
    _isheadSend = false;

}

void Response::makeHeader(int status)
{
    _header = "HTTP/1.1 " + std::to_string(status) + " " + _status[status] + "\r\n";
    _headers.erase(_headers.begin());
    while(_headers.size() > 0)
    {
        _header += _headers.begin()->first + ": " + _headers.begin()->second + "\r\n";
        _headers.erase(_headers.begin());
    }
}

void Response::sendHeaders(int fd)
{
    int ret;
    std::fstream  file;
    ret = send(fd,_header.c_str(),_header.length(),0);
    if(ret <= 0)
        return;
    _isheadSend = true;
    fileSend = &file;
    file.open(_filePath, std::ios::in);
}

void Response::sendBody(int fd)
{
    char *buffer = new char[1024];
    int ret;
    fileSend->read(buffer, 1024);
    ret = fileSend->gcount();
    if(ret <=  0)
    {
        delete[] buffer;
        fileSend->close();
        return;
    }
    ret = send(fd,buffer,ret,0);
    if(ret <= 0)
    {
        delete[] buffer;
        fileSend->close();
        return;
    }
}

void Response::sendRangeBody(int fd ,size_t start)
{
    char *buffer = new char[102400];
    int ret;
    fileSend->seekg(start);
    fileSend->read(buffer, 102400);
    ret = fileSend->gcount();
    if(ret <=  0)
    {
        delete[] buffer;
        fileSend->close();
        return;
    }
    _header = "HTTP/1.1 206 Partial Content\r\n";
    _header += "Content-Range: bytes " + std::to_string(start) + "-" + std::to_string(start + ret) + "/" + std::to_string(fileSend->tellg()) + "\r\n";
    _header += "Content-Length: " + std::to_string(ret) + "\r\n";
    _header += "Content-Type: " + _headers["Content-Type"] + "\r\n";
    _header += "Accept-Ranges: bytes\r\n";
    _header += "Connection: keep-alive\r\n"; 
    _header += "\r\n";
    _header.append(buffer, ret);
    ret = send(fd,_header.c_str(),_header.size(),0);
    if(ret <= 0)
    {
        delete[] buffer;
        fileSend->close();
        return;
    }
    delete[] buffer;
    fileSend->close();
}


