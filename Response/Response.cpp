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
    _status[413] = "Payload Too Large";
    _status[414] = "URI Too Long";
    _status[408] = "Request Timeout";
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
    _locationResponse = NULL;
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
    fileSend.open(_file, std::ios::in | std::ios::binary);
    // std::cout << "file : >>>>>>>>>>>>" << _file << std::endl;
    if(!S_ISREG(filestat.st_mode))
    {
        // std::cout << "<<<<<<<<<<<<<<<<,make body >>>>>>>>>>>" << std::endl;
        makeBody();
    }
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
    fileSend.read(buffer, 10240);
    ret = fileSend.gcount();
    if(ret <=  0 )
    {
        delete[] buffer;
        fileSend.close();
        
        _isBodyEnd = true;
        return;
    }
    ret = send(fd,buffer,ret,0);
    if(ret <= 0)
    {
        // std::cout << "ret2 : " << ret << std::endl;
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
    char *buffer = new char[102400];
    int ret;
    struct stat filestat;
    stat(_file.c_str(), &filestat);
    fileSend.open(_file, std::ios::in | std::ios::binary);
    if(!fileSend.is_open())
    {
        delete[] buffer;
        _isBodyEnd = true;
        return;
    }
    fileSend.seekg(start);
    fileSend.read(buffer, 102400);
    ret = fileSend.gcount();
    if(ret <=  0)
    {
        delete[] buffer;
        fileSend.close();
         _isBodyEnd = true;
        return;
    }
    _header = "HTTP/1.1 206 Partial Content\r\n";
    size_t pos = start + ret;
    if(pos >= static_cast<size_t>(filestat.st_size))
        pos = filestat.st_size - 1;
    _header += "Content-Type: video/mp4\r\n";
    _header += "Accept-Ranges: bytes\r\n";
    _header += "Connection: keep-alive\r\n"; 
    _header += "Content-Range: bytes " + std::to_string(start) + "-" + std::to_string(pos) + "/" + std::to_string(filestat.st_size) + "\r\n";
    _header += "Content-Length: " + std::to_string(ret) + "\r\n";
    _header += "\r\n";
    _header.append(buffer, ret);
    ret = send(fd,_header.c_str(),_header.size(),0);
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
        _bodyResponse = main._bodyResponse;
        _locationResponse = main._locationResponse;
    }
    return *this;
}


void Response::fillResponseMap()
{
    _headersResponse["Content-Type"] = "text/html";  
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
    if(_bodyResponse.size() > 0)
    {
        _headersResponse["Content-Length"] = std::to_string(_bodyResponse.size());
        // std::cout << "body size : >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << _bodyResponse.size() << std::endl;
    }
    else if(_headersRequest.find("Range") != _headersRequest.end())
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

 void Response::sendBodyString(int fd)
 {
        int ret;
        std::string tmp;
        if(_bodyResponse.size() > 1024)
        {
            tmp = _bodyResponse.substr(0,1024);
            _bodyResponse = _bodyResponse.substr(1024);
            ret = send(fd,_bodyResponse.c_str(),1024,0);
            if(ret <= 0 || _bodyResponse.size() == 0)
            {
                _isBodyEnd = true;
                _isConnectionClose = true;
            }
            
        }else 
        {
            ret = send(fd,_bodyResponse.c_str(),_bodyResponse.size(),0);
            if(ret <= 0)
            {
                _isBodyEnd = true;
                _isConnectionClose = true;
            }
            _bodyResponse.clear();
        }
 }


void Response::makeBody()
{
    struct stat filestat;
    struct dirent *en;
    //std::string tmp = _headersRequest["Path"].substr(0, _headersRequest["Path"].find_last_of('/') + 1);
    if(_bodyResponse.size() > 0)
        return;
    if(_headersRequest["Path"].back() != '/')
        _headersRequest["Path"] += "/";
    stat(_file.c_str(), &filestat);
    if (_statusCode == 200 && S_ISDIR(filestat.st_mode) && _locationResponse->isReturn   == false)              
    {
        DIR *dir = opendir(_file.c_str());
        _bodyResponse  = "<html>\n<head>\n<title>Index of " + _headersRequest["Path"] + "</title>\n</head>\n<body>\n<h1>Index of " + _headersRequest["Path"] + "</h1>\n";
        while((en =readdir(dir)) != NULL)
        {
            if(std::string(en->d_name).front() == '.')
                continue;
            _bodyResponse += "<a href=\"" + _headersRequest["Path"] + std::string(en->d_name) + "\">" + std::string(en->d_name) + "</a><br>";
        }
        _bodyResponse += "</body>\n</html>";
        closedir(dir);
    }else
    {
        //makeErroPage(_bodyResponse, _status[_statusCode], _statusCode);
        _bodyResponse = "<html>\n<head>\n<title>" + std::to_string(_statusCode) + " " + _status[_statusCode] + "</title>\n</head>\n<body>\n<h1>" + std::to_string(_statusCode) + " " + _status[_statusCode] + "</h1>" + "\n</body>\n</html>";
    }
}


void makeErroPage(std::string &body,std::string message , int status)
{
    std::string tmp;
    tmp = "<!DOCTYPE html>\n"
                "<html lang=\"en\">\n"
                "\n"
                "<head>\n"
                "    <meta charset=\"utf-8\">\n"
                "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n"
                "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
                "    \n"
                "\n"
                "    <title>404 HTML Template by Colorlib</title>\n"
                "\n"
                "    \n"
                "    <link href=\"https://fonts.googleapis.com/css?family=Montserrat:700,900\" rel=\"stylesheet\">\n"
                "\n"
                "    \n"
                "    <style>\n"
                "        * {\n"
                "            -webkit-box-sizing: border-box;\n"
                "            box-sizing: border-box;\n"
                "        }\n"
                "\n"
                "        body {\n"
                "            padding: 0;\n"
                "            margin: 0;\n"
                "        }\n"
                "\n"
                "        #notfound {\n"
                "            position: relative;\n"
                "            height: 100vh;\n"
                "            background: #030005;\n"
                "            font-family: 'Montserrat', sans-serif;\n"
                "        }\n"
                "\n"
                "        #notfound .notfound {\n"
                "            position: absolute;\n"
                "            left: 50%;\n"
                "            top: 50%;\n"
                "            -webkit-transform: translate(-50%, -50%);\n"
                "            -ms-transform: translate(-50%, -50%);\n"
                "            transform: translate(-50%, -50%);\n"
                "        }\n"
                "\n"
                "        .notfound {\n"
                "            max-width: 767px;\n"
                "            width: 100%;\n"
                "            line-height: 1.4;\n"
                "            text-align: center;\n"
                "        }\n"
                "\n"
                "        .notfound .notfound-404 {\n"
                "            position: relative;\n"
                "            height: 180px;\n"
                "            margin-bottom: 20px;\n"
                "            z-index: -1;\n"
                "        }\n"
                "\n"
                "        .notfound .notfound-404 h1 {\n"
                "\n"
                "        #notfound .notfound {\n"
                "            position: absolute;\n"
                "            left: 50%;\n"
                "            top: 50%;\n"
                "            -webkit-transform: translate(-50%, -50%);\n"
                "            -ms-transform: translate(-50%, -50%);\n"
                "            transform: translate(-50%, -50%);\n"
                "        }\n"
                "\n"
                "        .notfound {\n"
                "            max-width: 767px;\n"
                "            width: 100%;\n"
                "            line-height: 1.4;\n"
                "            text-align: center;\n"
                "        }\n"
                "\n"
                "        .notfound .notfound-404 {\n"
                "            position: relative;\n"
                "            height: 180px;\n"
                "            margin-bottom: 20px;\n"
                "            z-index: -1;\n"
                "        }\n"
                "\n"
                "        .notfound .notfound-404 h1 {\n"
                "            font-family: 'Montserrat', sans-serif;\n"
                "            position: absolute;\n"
                "            left: 50%;\n"
                "            top: 50%;\n"
                "            -webkit-transform: translate(-50%, -50%);\n"
                "            -ms-transform: translate(-50%, -50%);\n"
                "            transform: translate(-50%, -50%);\n"
                "            font-size: 224px;\n"
                "            font-weight: 900;\n"
                "            margin-top: 0px;\n"
                "            margin-bottom: 0px;\n"
                "            margin-left: -12px;\n"
                "            color: #030005;\n"
                "            text-transform: uppercase;\n"
                "            text-shadow: -1px -1px 0px #8400ff, 1px 1px 0px #ff005a;\n"
                "            letter-spacing: -20px;\n"
                "        }\n"
                "\n"
                "        .notfound .notfound-404 h2 {\n"
                "            font-family: 'Montserrat', sans-serif;\n"
                "            position: absolute;\n"
                "            left: 0;\n"
                "            right: 0;\n"
                "            top: 110px;\n"
                "            font-size: 42px;\n"
                "            font-weight: 700;\n"
                "            color: #fff;\n"
                "            text-transform: uppercase;\n"
                "            text-shadow: 0px 2px 0px #8400ff;\n"
                "            letter-spacing: 13px;\n"
                "            margin: 0;\n"
                "        }\n"
                "\n"
                "        .notfound a {\n"
                "            font-family: 'Montserrat', sans-serif;\n"
                "            display: inline-block;\n"
                "            text-transform: uppercase;\n"
                "            color: #ff005a;\n"
                "            text-decoration: none;\n"
                "            border: 2px solid;\n"
                "            background: transparent;\n"
                "            padding: 10px 40px;\n"
                "            font-size: 14px;\n"
                "            font-weight: 700;\n"
                "            -webkit-transition: 0.2s all;\n"
                "            transition: 0.2s all;\n"
                "        }\n"
                "\n"
                "        .notfound:hover a {\n"
                "            color: #8400ff;\n"
                "        }\n"
                "\n"
                "        @media only screen and (max-width: 767px) {\n"
                "            .notfound .notfound-404 h2 {\n"
                "                font-size: 24px;\n"
                "            }\n"
                "        }\n"
                "\n"
                "        @media only screen and (max-width: 480px) {\n"
                "            .notfound .notfound-404 h1 {\n"
                "                font-size: 182px;\n"
                "            }\n"
                "        }\n"
                "\n"
                "    </style>\n"
                "\n"
                "</head>\n"
                "\n"
                "<body>\n"
                "\n"
                "    <div id=\"notfound\">\n"
                "        <div class=\"notfound\">\n"
                "            <div class=\"notfound-404\">\n"
                "                <h1>" + std::to_string(status) + "</h1>\n"
                "            </div>\n"
                "            <h2>" + message + "</h2>\n"
                "            <a href=\"/\">Homepage</a>\n"
                "        </div>\n"
                "    </div>\n"
                "\n"
                "</body>\n"
                "\n"
                "</html>";
    body = tmp;
}
