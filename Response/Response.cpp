#include "Response.hpp"
#include <sys/stat.h>
#include <dirent.h>
#include <sstream>
#include "../server/server.hpp"

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
    _status[406] = "Not Acceptable";
    _status[408] = "Request Timeout";
    _status[409] = "Conflict";
    _status[411] = "Length Required";
    _status[413] = "Payload Too Large";
    _status[414] = "URI Too Long";
    _status[408] = "Request Timeout";
    _status[500] = "Internal Server Error";
    _status[501] = "Not Implemented";
    _status[505] = "HTTP Version Not Supported";
    _contentLengthSend = 0;


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
    _MimeType[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    _MimeType[".xls"] = "application/vnd.ms-excel";
    _MimeType[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    _MimeType[".ppt"] = "application/vnd.ms-powerpoint";
    _MimeType[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    _MimeType[".odt"] = "application/vnd.oasis.opendocument.text";
    _MimeType[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
    _MimeType[".odp"] = "application/vnd.oasis.opendocument.presentation";
    _MimeType[".odg"] = "application/vnd.oasis.opendocument.graphics";
    _MimeType[".odc"] = "application/vnd.oasis.opendocument.chart";
    _MimeType[".odb"] = "application/vnd.oasis.opendocument.database";
    _MimeType[".odf"] = "application/vnd.oasis.opendocument.formula";
    _MimeType[".wpd"] = "application/vnd.wordperfect";
    _MimeType[".iso"] = "application/x-iso9660-image";
    _MimeType[".csv"] = "text/csv";
    _MimeType[".rtf"] = "application/rtf";
    _MimeType[".7z"] = "application/x-7z-compressed";
    _MimeType[".ics"] = "text/calendar";
    _MimeType[".vcf"] = "text/x-vcard";
    _MimeType[".apk"] = "application/vnd.android.package-archive";
    _MimeType[".bat"] = "application/x-msdownload";
    _MimeType[".bin"] = "application/octet-stream";
    _MimeType[".cgi"] = "application/octet-stream";
    _MimeType[".pl"] = "application/x-perl";
    _MimeType[".py"] = "application/x-python";
    _MimeType[".sh"] = "application/x-sh";
    _MimeType[".sql"] = "application/x-sql";
    _MimeType[".cab"] = "application/vnd.ms-cab-compressed";
    _MimeType[".deb"] = "application/x-deb";
    _MimeType[".tar.gz"] = "application/x-tar";
    _MimeType[".tar.bz2"] = "application/x-bzip-compressed-tar";
    _MimeType[".tar.lzma"] = "application/x-lzma-compressed-tar";
    _MimeType[".tar.xz"] = "application/x-xz-compressed-tar";
    _MimeType[".gem"] = "application/x-ruby";
    _MimeType[".rpm"] = "application/x-rpm";
    _MimeType[".rss"] = "application/rss+xml";
    _MimeType[".dmg"] = "application/x-apple-diskimage";
    _isheadSend = false;
    _isBodyEnd = false;
    _isConnectionClose = false;
    isBodyString = false;
    _locationResponse = NULL;
}

void Response::makeHeader(int status)
{
    _header = "HTTP/1.1 " + std::to_string(status) + " " + _status[status] + "\r\n";
    for(size_t i = 0; _headersCgi.size() > i; i++)
        _header += _headersCgi.at(i).first + ": " + _headersCgi.at(i).second + "\r\n";
    while(_headersResponse.size() > 0)
    {
        if(_header.find(_headersResponse.begin()->first) == std::string::npos) 
            _header += _headersResponse.begin()->first + ": " + _headersResponse.begin()->second + "\r\n";
        _headersResponse.erase(_headersResponse.begin());
    }
    _header += "\r\n";
}

void Response::sendHeaders(int fd)
{

     if(_headersRequest.find("Range") != _headersRequest.end())
    {
        std::string tmp = _headersRequest["Range"];
        tmp = tmp.substr(tmp.find("=") + 1);
        size_t pos = 0;
        std::stringstream ss;
        ss << tmp;
        ss >> pos;
        sendRangeBody(fd, pos);
        return;
    }
    int ret;
    struct stat filestat;
    stat(_file.c_str(), &filestat);
    fillResponseMap();
    if(!S_ISREG(filestat.st_mode))
        makeBody();
    createLengthHeader();
    fileSend.close();
    makeHeader(_statusCode);
    ret = send(fd,_header.c_str(),_header.size(),0);
    if(ret <= 0)
    {
        _isBodyEnd = true;
        _isConnectionClose = true;
        return;
    }
    _isheadSend = true;
}

void Response::sendBody(int fd)
{
    char buffer[30000];
    int ret = 0;
    fileSend.open(_file, std::ios::in );
    fileSend.seekg(_contentLengthSend);
    fileSend.read(buffer, 30000); // Read up to 1500 bytes from the file

    ret = fileSend.gcount(); // Get the number of bytes read
    if (ret <= 0)
    {
        struct stat filestat;
        stat(_file.c_str(), &filestat);
        if(_contentLengthSend != static_cast<size_t>(filestat.st_size))
            return;
        fileSend.close();
        _isBodyEnd = true;
        // _isConnectionClose = true;
        return;
    }
    ret = send(fd, buffer, ret, 0);
    if (ret <= 0)
    {
        fileSend.close();
        _isBodyEnd = true;
        _isConnectionClose = true;
        return;
    }
    _contentLengthSend += ret;
    fileSend.close();
}
std::string getDate()
{
    std::time_t crnt = time(NULL);
    char buffer[1000];
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&crnt));
    return std::string(buffer);
}

void Response::sendRangeBody(int fd ,size_t start)
{
    char buffer[202400];
    int ret;
    struct stat filestat;
    std::string tmp;
    stat(_file.c_str(), &filestat);
    fileSend.open(_file, std::ios::in);
    if(!fileSend.is_open())
    {
        _isBodyEnd = true;
        return;
    }
    fileSend.seekg(start);
    fileSend.read(buffer, 202400);
    ret = fileSend.gcount();
    if(ret <=  0)
    {
        fileSend.close();
         _isBodyEnd = true;
        return;
    }
    _header = "HTTP/1.1 206 Partial Content\r\n";
    size_t pos = start + ret;
    if(pos >= static_cast<size_t>(filestat.st_size))
        pos = filestat.st_size - 1;
    tmp = "Content-Type: application/octet-stream\r\n"; 
    if(_file.find_last_of('.') != std::string::npos)
        if(_MimeType.find(_file.substr(_file.find_last_of('.'))) != _MimeType.end())
            tmp = "Content-Type: " +  _MimeType[_file.substr(_file.find_last_of('.'))] + "\r\n"; 
    _header += tmp;
    _header += "Accept-Ranges: bytes\r\n";
    _header += "Connection: close\r\n"; 
    _header += "Content-Range: bytes " + std::to_string(start) + "-" + std::to_string(pos) + "/" + std::to_string(filestat.st_size) + "\r\n";
    _header += "Content-Length: " + std::to_string(ret) + "\r\n";
    _header += "Server: " + _servername + "\r\n";
    _header += "cache-control: private ,Max-Age=3600\r\n";
    _header += "Date: " + getDate() + "\r\n";
    _header += "\r\n";
    _header.append(buffer, ret);
    ret = send(fd,_header.c_str(),_header.size(),0);
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
        _bodyResponse = main._bodyResponse;
        _locationResponse = main._locationResponse;
        _servername = main._servername;
        _contentLengthSend = main._contentLengthSend;
        _headersCgi = main._headersCgi;
        isBodyString = main.isBodyString;
    }
    return *this;
}

std::string lastTime(std::time_t crnt)
{
    char buffer[1000];
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&crnt));
    return std::string(buffer);
}
void Response::fillResponseMap()
{
    struct stat filestat;
    stat(_file.c_str(), &filestat);
    _headersResponse["Content-Type"] = "text/html"; 
    if(_statusCode == 405)
    {
        std::string tmp;
        for(size_t i = 0;_locationResponse->methods.size() > i; i++)
            tmp += _locationResponse->methods.at(i) + ", ";
        _headersResponse["Allow"] = tmp.substr(0,tmp.size() - 2);
    }
    if(_file.find_last_of('.') != std::string::npos)
    {
        if(_MimeType.find(_file.substr(_file.find_last_of('.'))) != _MimeType.end())
            _headersResponse["Content-Type"] = _MimeType[_file.substr(_file.find_last_of('.'))];
        else
            _headersResponse["Content-Type"] = "application/octet-stream";  
    }
    if(S_ISREG(filestat.st_mode))
        _headersResponse["Last-Modified"] = lastTime(filestat.st_mtime);
    _headersResponse["Server"] = _servername;
    _headersResponse["Date"] =  getDate();
    _headersResponse["Accept-Ranges"] = "bytes";

    if(_headersRequest.find("Connection") != _headersRequest.end() && _headersRequest["Connection"] == "close")
        _headersResponse["Connection"] = "close";
    else
        _headersResponse["Connection"] = "keep-alive";
    _headersResponse["cache-control"] = "no-store";

}
void  Response::sendExaption(int fd, int status)
{
    (void) fd;
    (void) status;
}

void Response::createLengthHeader()
{
    struct stat filestat;
    stat(_file.c_str(), &filestat);
    if(isBodyString == true)
        _headersResponse["Content-Length"] = std::to_string(_bodyResponse.size());
    else
        _headersResponse["Content-Length"] = std::to_string(filestat.st_size);
}



 void Response::sendBodyString(int fd)
 {
        int ret = 0;
        std::string tmp;
        if(_bodyResponse.size() > 5000)
        {
            tmp = _bodyResponse.substr(0,1024);
            ret = send(fd,tmp.c_str(),1024,0);
            if(ret <= 0)
            {
                _isBodyEnd = true;
                _isConnectionClose = true;
                return;
            }
            _bodyResponse = _bodyResponse.substr(ret);
        }else 
        {
            if(_bodyResponse.size() == 0)
            {
                _isBodyEnd = true;
                return;
            }
            ret = send(fd,_bodyResponse.c_str(),_bodyResponse.size(),0);
            _bodyResponse = _bodyResponse.substr(ret);
            if(ret <= 0 )
            {
                _isBodyEnd = true;
                _isConnectionClose = true;
                return;
            }
            _bodyResponse.clear();
        }
 }

void Response::makeBody()
{
    struct stat filestat;
    struct dirent *en;
    if(_bodyResponse.size() > 0)
        return;
    if(_headersRequest["Path"].back() != '/')
        _headersRequest["Path"] += "/";
    stat(_file.c_str(), &filestat);
    if (_statusCode == 200 && S_ISDIR(filestat.st_mode) && _locationResponse->isReturn   == false)              
    {
        DIR *dir = opendir(_file.c_str());
        _bodyResponse  = "<html>\n<head>\n<title>Index of " + _headersRequest["Path"] + "</title>\n</head>\n<body>\n<h1 align=\"center\">Index of " + _headersRequest["Path"] + "</h1>\n";
        _bodyResponse += "<hr>\n<pre>\n";
        while((en =readdir(dir)) != NULL)
        {
            if(std::string(en->d_name).front() == '.')
                continue;
            _bodyResponse += "<a href=\"" + _headersRequest["Path"] + std::string(en->d_name) + "\">" +std::string(en->d_name) + "</a> <br>";
        }
        _bodyResponse += "</pre>\n<hr>\n";
        _bodyResponse += "</body>\n</html>";
        closedir(dir);
    }else
    {
        _bodyResponse = "<html>\n<head>\n<title>" + std::to_string(_statusCode) + " " + _status[_statusCode] + "</title>\n</head>\n<body>\n <h1 >"  + "<h1 style=\"margin: 40px; \"align=\"center\">" + std::to_string(_statusCode) + " " + _status[_statusCode] + "</h1>\n";
         _bodyResponse += "<hr>\n<pre>\n";
         _bodyResponse += "<h4 align=\"center\">Server: " + _servername + "</h4>\n";
        _bodyResponse += "</body>\n</html>";
    }
}