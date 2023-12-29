#include "Client.hpp"

Client::Client() : Request(), Response()
{
    _isparsed = false;
    _isReadBody = false;
    _server = NULL;
    _servers = NULL;
    _chunkedSize = 0 ;
    _isreadFromClient = false;
    _currentLength = 0;
    isBodyString = false;
}
Client::~Client()
{

}

void Client::readRequest()
{
    _isreadFromClient = false;
    if (_isparsed == false)
        readheader();
    if (_isReadBody == true)
        readbody();
    if(_isparsed == true &&  _isCgi == true  && _isReadBody == false)
    {
        _cgi = new cgi(_locationCgi->compiler, _pathFile, _request);
        std::string tmp;
        if(_cgi->statusCode != 0)
            throw (delete _cgi ,_errorCode = 500, _isError = true, "return");
        tmp = _cgi->getResponse();
        if(tmp.size() > 0 && tmp.find("\r\n\r\n") != std::string::npos)
        {
            _body = tmp.substr(tmp.find("\r\n\r\n") + 4);
            tmp = tmp.substr(0, tmp.find("\r\n\r\n")+4);
            while(tmp != "\r\n" && tmp != "")
            {

                _headersCgi.push_back(std::make_pair(tmp.substr(0,tmp.find(":")), tmp.substr(tmp.find(":") + 2, tmp.find("\r\n")-tmp.find(":") - 2)));
                tmp = tmp.substr(tmp.find("\r\n") + 2);
            }
        }else
            _body = _cgi->getResponse();
        delete _cgi;
        _isCgi = false;
        _locationResponse = _locationCgi;
    }
}

void Client::readheader()
{
    char buffer[1000];
    std::string tmp;
    int ret = 1;
    if (!_headerIsRecv)
    {
        _isreadFromClient = true;
        ret = recv(_socket, buffer, 1000, 0);
        if (ret <= 0)
            return;
        _request.append(buffer, ret);
        timestamp = getTime();
    }
    timestamp = time(NULL);
    if (_request.find("\r\n\r\n") == std::string::npos)
        return;
    _isparsed = true;
    ParseRequest(_request.substr(0, _request.find("\r\n\r\n") + 4));
    _server = &findServer();
    _body = _request.substr(_request.find("\r\n\r\n") + 4);
    _request = _request.substr(0, _request.find("\r\n\r\n") + 4);
    checkRequest();
    uriToPath();
    _headersRequest = _headers;
    tmp = _headers["Path"];
    if(tmp.size() >= 3 && (tmp.substr(tmp.size() - 3) == ".py" || tmp.substr(tmp.size() - 3) == ".js" ))
        matchCgi();
    if(_isCgi == true)
        return;
    matchlocation();
    _file = _pathFile;
}

void Client::readHyprid(int fd)
{   
    char buffer[3000];
    int ret = 1;
    std::stringstream ss;
    if(_isreadFromClient == false)
    {
        ret = recv(fd, buffer, 3000, 0);
        if(ret <= 0)
            return;
        _body.append(buffer, ret);
        _isreadFromClient = true;
        timestamp = time(NULL);
    }

   while(_body.size() > 0)
   {
        if(_chunkedSize == 0 && _body.find("\r\n") != std::string::npos)
        {
            if(_body.find("0\r\n\r\n") == 0)
            {
                 
                _body.erase(0, _body.find("\r\n\r\n") + 4);
                isBodyEnd = true;
                break;
            }
            ss << std::hex << _body.substr(0, _body.find("\r\n"));
            ss >> _chunkedSize;
            _body.erase(0, _body.find("\r\n") + 2);
            ss.clear();
        }
        if(_chunkedSize > 0 && _body.size() > 0)
        {
            if(_body.size() >= _chunkedSize + 2)
            {
                _bodyHyprid += _body.substr(0, _chunkedSize);
                _body.erase(0, _chunkedSize + 2);
                _chunkedSize = 0;
            }else
                break;
        }else
            break;
   }

    std::string midBoundary = "--" + _boundry + "\r\n";
    std::string endBoundary = "--" + _boundry + "--\r\n";
   
    while( _bodyHyprid.size() > 0)
    {
        if(_isNotFirst == false && _bodyHyprid.find(midBoundary) == 0 && _bodyHyprid.find("\r\n\r\n") != std::string::npos) 
        {
            _fileupload = getFileName(_bodyHyprid.substr(0, _bodyHyprid.find("\r\n\r\n")));
            _filesUploaded.push_back(_fileupload);
            _bodyHyprid.erase(0, _bodyHyprid.find("\r\n\r\n") + 4);
            _isNotFirst = true;
        }
        if(_isNotFirst == true)
        {
            if(_bodyHyprid.find(midBoundary) != std::string::npos)
            {
                std::ofstream outfile(_fileupload.c_str(), std::ios::app);
                if (!outfile.is_open())
                {
                    remove(_fileupload.c_str());
                    throw (_errorCode = 501,_isError = true ,"return"); // Bad Request
                }
                outfile << _bodyHyprid.substr(0, _bodyHyprid.find(midBoundary));
                _currentLength += _bodyHyprid.substr(0, _bodyHyprid.find(midBoundary)).size();
                if(_currentLength > _location->max_body_size)
                {
                    for(size_t i = 0; i < _filesUploaded.size(); i++)
                        remove(_filesUploaded[i].c_str());
                    throw (_errorCode = 413 ,"return"); // Request Entity Too Large
                }
                outfile.close();
                _bodyHyprid.erase(0, _bodyHyprid.find(midBoundary));
                _isNotFirst = false;
            }
            else if(_bodyHyprid.find(endBoundary) != std::string::npos)
            {
                std::ofstream outfile(_fileupload.c_str(), std::ios::app);
                if (!outfile.is_open())
                {
                    remove(_fileupload.c_str());
                    throw (_errorCode = 501,_isError = true ,"return"); // Bad Request
                }
                outfile << _bodyHyprid.substr(0, _bodyHyprid.find(endBoundary));
                _currentLength += _bodyHyprid.substr(0, _bodyHyprid.find(endBoundary)).size();
                if(_currentLength > _location->max_body_size)
                {
                    for(size_t i = 0; i < _filesUploaded.size(); i++)
                        remove(_filesUploaded[i].c_str());
                    throw (_errorCode = 413 ,"return"); // Request Entity Too Large
                }
                outfile.close();
                _bodyHyprid.erase(0, _bodyHyprid.find(endBoundary));
                _bodyHyprid.clear();
                isBodyEnd = true;
                throw(_errorCode = 201, _isError = true, "binary");
            }
            else if(_bodyHyprid.size() >= 2000)
            {
                std::ofstream outfile(_fileupload.c_str(), std::ios::app);
                if (!outfile.is_open())
                {
                    remove(_fileupload.c_str());
                    throw (_errorCode = 501,_isError = true ,"return"); // Bad Request
                }
                outfile << _bodyHyprid.substr(0, _bodyHyprid.size() - 1000);
                _currentLength += _bodyHyprid.substr(0, _bodyHyprid.size() - 1000).size();
                if(_currentLength > _location->max_body_size)
                {
                    for(size_t i = 0; i < _filesUploaded.size(); i++)
                        remove(_filesUploaded[i].c_str());
                    throw (_errorCode = 413 ,"return"); // Request Entity Too Large
                }
                outfile.close();
                _bodyHyprid.erase(0, _bodyHyprid.size() - 1000);
            }
            else
                break;
        }else
            break;
    }
}

void Client::readbody()
{
       
    if (isBodyEnd == false && _isCgi == false)
    {
        if(getHeader("Transfer-Encoding") != NULL && getHeader("Content-Type") != NULL && getHeader("Content-Type")->find("boundary=") != std::string::npos)
            readHyprid(_socket);
        else if (getHeader("Transfer-Encoding") != NULL)
            readChunked(_socket);
        else if (getHeader("Content-Type") != NULL && getHeader("Content-Type")->find("boundary=") != std::string::npos)
            readBoundry(_socket);
        else
            readContentLength(_socket);

    }
    if(_isCgi == true)
    {

        if (isBodyEnd == false)
        {
            if (getHeader("Transfer-Encoding") != NULL)
                readChunkedCgi(_socket);
            else if (getHeader("Content-Type") != NULL && getHeader("Content-Type")->find("boundary=") != std::string::npos)
                readBoundryCgi(_socket);
            else
                readContentLengthCgi(_socket);
        }
        if(isBodyEnd == true)
        {
            _request += _body;
            _isReadBody = false;
        }
    }
}

void Client::sendResponse()
{
    signal(SIGPIPE, SIG_IGN);
    if (_errorCode == 0)
        _errorCode = 200;
    _statusCode = _errorCode;
    if (_isheadSend == false)
        sendHeaders(_socket);
    else if (isBodyString == true)
        sendBodyString(_socket);
    else
        sendBody(_socket);
}

server &Client::findServer()
{
    std::vector<server *> tmpServer;
    std::string tmp_host;
    std::string tmp_port;
    std::string *_host = getHeader("Host");
    if (_host != NULL)
    {
        tmp_host = _host->substr(0, _host->find(":"));
        tmp_port = _host->substr(_host->find(":") + 1);
    }
    for (size_t i = 0; i < _servers->size(); i++)
        if (std::to_string(_servers->at(i).getPort()) == tmp_port || _servers->at(i).fd == serverFd)
            tmpServer.push_back(&_servers->at(i));
    for (size_t i = 0; i < tmpServer.size(); i++)
    {
        for (size_t j = 0; j < tmpServer[i]->getServerNames().size(); j++)
        {
            if (tmpServer[i]->getServerNames()[j] == tmp_host)
            {
                _servername = tmpServer[i]->getServerNames()[j];
                return *tmpServer[i];
            }
        }
    }
    _servername = tmpServer[0]->getServerNames()[0];
    return *tmpServer[0];
}

bool Client::getIsParsed() const
{
    return _isparsed;
}

bool Client::checkReturn()
{
    std::vector<int> numbers;
    std::string tmp;
    numbers.push_back(301), numbers.push_back(302), numbers.push_back(303);
    if (_location == NULL || _location->isReturn == false)
        return false;
    if (_location->_return.first != -1 && _location->_return.second != "")
    {
        if (std::find(numbers.begin(), numbers.end(), _location->_return.first) != numbers.end())
        {
            _errorCode = _location->_return.first;
            if (_location->_return.second.front() == '/' && _headers.find("Host") != _headers.end())
                _headersResponse["Location"] = "http://" + _headers["Host"] + _location->_return.second;
            else
                _headersResponse["Location"] = _location->_return.second;
        }
        else
        {
            _errorCode = _location->_return.first;
            _body = _location->_return.second;
        }
    }
    else if (_location->_return.first != -1 && _location->_return.second == "")
    {
        _errorCode = _location->_return.first;
    }
    else if (_location->_return.first == -1 && _location->_return.second != "")
    {
        _headersResponse["Location"] = _location->_return.second;
        _errorCode = 301;
    }
    return true;
}
void Client::parseHyprid()
{
    _boundry = _headers["Content-Type"].substr(_headers["Content-Type"].find("boundary=") + 9);
    _body = unchunk(_body);
    // parseMultipartData();
}



char randchar()
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    return alphanum[rand() % (sizeof(alphanum) - 1)];
}

std::string Client::generateRandomString()
{
    size_t length = 10;
    
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}





std::string Client::unchunk(std::string &chunked)
{
    std::string unchunkedData, length_str;
    size_t length, pos = 0;
    while ((pos = chunked.find("\r\n")) != std::string::npos)
    {
        length_str = chunked.substr(0, pos);
        length = std::stoul(length_str, nullptr, 16);

        if (length == 0)
            break;

        chunked.erase(0, pos + 2);
        unchunkedData.append(chunked, 0, length);
        chunked.erase(0, length + 2);
    }

    return unchunkedData;
}



size_t Client::fileCount()
{
    size_t count = 0;
    size_t pos = 0;
    std::string tkn = "--" + _boundry + "\r\n";
    while ((pos = _body.find(tkn, pos)) != std::string::npos)
    {
        count++;
        pos += tkn.length();
    }
    return count;
}

std::string Client::getFileName(std::string fileHeader)
{
    std::string filename = "";
    std::string extension;
    std::string content_type;
    struct stat filehelp;
    size_t pos = 0;
    if ((pos = fileHeader.find("Content-Type: ")) != std::string::npos)
    {
        pos += 14;
        content_type = fileHeader.substr(pos, fileHeader.find("\r\n", pos) - pos);
        if ((pos = content_type.find("/")) != std::string::npos)
            extension = "." + content_type.substr(pos + 1);
        else
            extension = ".txt";
    }
    else
        extension = ".txt";

    if ((pos = fileHeader.find("filename=")) != std::string::npos)
    {
        filename = fileHeader.substr(pos + 10);
        filename = filename.substr(0, filename.find("\""));
         if (filename == "")
            filename = generateRandomString();
    }
    else if ((pos = fileHeader.find("name=")) != std::string::npos && fileHeader.find("filename=") == std::string::npos)
    {
        filename = fileHeader.substr(pos + 6);
        filename = filename.substr(0, filename.find("\""));
        if (filename == "")
            filename = generateRandomString();
        filename += extension;
    }
    else
        filename = generateRandomString() + extension;

    if (_pathFile.back() != '/')
        filename = _pathFile + "/" + filename;
    else
        filename = _pathFile + filename;
    stat(filename.c_str(), &filehelp);
    if (S_ISREG(filehelp.st_mode))
        throw(_errorCode = 409, _isError = true, "no idont think so");
    return filename;
}





void  Client::readContentLength(int fd)
{
    char buffer[5000];
    struct stat _stat;
    int ret = 1;
    if(_isNotFirst == false)
    {
        parseBinaryData();
        _isNotFirst = true;
    }
    if(_isreadFromClient == false)
    {
        ret = recv(fd, buffer, 5000, 0);
        if(ret <= 0)
           return;
        _body.append(buffer, ret);
        timestamp = time(NULL);
        _isreadFromClient = true;
    }
    std::ofstream outfile(_fileupload.c_str(), std::ios::app);
    if (!outfile.is_open())
    {
        remove(_fileupload.c_str());
        throw (_errorCode = 500,_isError = true ,"return"); // Bad Request
    }
    outfile << _body;
    outfile.close();
    _body.clear();
    stat(_fileupload.c_str(), &_stat);
    if(static_cast<size_t> (_stat.st_size) == _contentLength)
    { 
        isBodyEnd = true;
        throw(_errorCode = 201, _isError = true, "binary");
    }
    else if(static_cast<size_t> (_stat.st_size) > _contentLength || _contentLength > _location->max_body_size)
    {
        remove(_fileupload.c_str());
        throw (_errorCode = 413 ,"return"); // Request Entity Too Large
    }
}


void Client::parseBinaryData()
{
    struct stat filehelp;
    stat(_pathFile.c_str(), &filehelp);
    if(S_ISDIR(filehelp.st_mode))
    {
        _fileupload = _location->root;
        if (_fileupload.back() != '/')
            _fileupload += '/';
        filename = generateRandomString();
        _fileupload += filename;
    }
    else if (stat(_pathFile.c_str(), &filehelp) == 0)
        throw(_errorCode = 409, _isError = true, "no idont think so");
    else
        _fileupload = _pathFile;
    std::ofstream outfile(_fileupload.c_str(), std::ios::app);
    if (!outfile.is_open())
        throw (_errorCode = 500,_isError = true ,"return");
    outfile.close(); // Bad Request
}

void Client::readChunked(int fd)
{
    if(_isNotFirst == false)
    {
   
        parseBinaryData();
        _isNotFirst = true;
    }
    char buffer[3000];
    int ret = 1;
    std::stringstream ss;
    struct stat _stat;
    if(_isreadFromClient == false)
    {
        ret = recv(fd, buffer, 3000, 0);
        if(ret <= 0)
            return;
        _body.append(buffer, ret);
        timestamp = time(NULL);
        _isreadFromClient = true;
    }
   while(_body.size() > 0)
   {
        if(_chunkedSize == 0 && _body.find("\r\n") != std::string::npos)
        {
            if(_body.find("0\r\n\r\n") == 0)
            {
                 
                _body.erase(0, _body.find("\r\n\r\n") + 4);
                isBodyEnd = true;
                throw(_errorCode = 201, _isError = true, "binary");
            }
            ss << std::hex << _body.substr(0, _body.find("\r\n"));
            ss >> _chunkedSize;
            _body.erase(0, _body.find("\r\n") + 2);
            ss.clear();
        }
        if(_chunkedSize > 0 && _body.size() > 0)
        {
            if(_body.size() >= _chunkedSize + 2)
            {
                std::ofstream outfile(_fileupload.c_str(), std::ios::app);
                if (!outfile.is_open())
                {
                    remove(_fileupload.c_str());
                    throw (_errorCode = 500,_isError = true ,"return"); // Bad Request
                }
                
                outfile << _body.substr(0, _chunkedSize);
                stat(_fileupload.c_str(), &_stat);
                if(static_cast<size_t> (_stat.st_size) > _location->max_body_size)
                {
                    remove(_fileupload.c_str());
                    throw (_errorCode = 413 ,"return"); // Request Entity Too Large
                }
                outfile.close();
                _body.erase(0, _chunkedSize + 2);
                _chunkedSize = 0;
            }else
                break;
        }else
            break;
   }
}


void Client::readBoundry(int fd)
{
    char buffer[3040];
    int ret = 1;
    if(_isreadFromClient == false)
    {
        ret = recv(fd, buffer, 3040, 0);
        if(ret <= 0)
            return ;
        _body.append(buffer, ret);
        timestamp = time(NULL);
    }
    std::string midBoundary = "--" + _boundry + "\r\n";
    std::string endBoundary = "--" + _boundry + "--\r\n";
    while( _body.size() > 0)
    {

        if(_isNotFirst == false && _body.find(midBoundary) == 0 && _body.find("\r\n\r\n") != std::string::npos) 
        {
            _fileupload = getFileName(_body.substr(0, _body.find("\r\n\r\n")));
            _filesUploaded.push_back(_fileupload);
            _body.erase(0, _body.find("\r\n\r\n") + 4);
            _isNotFirst = true;
        }
        if(_isNotFirst == true)
        {
            if(_body.find(midBoundary) != std::string::npos)
            {
                std::ofstream outfile(_fileupload.c_str(), std::ios::app | std::ios::binary);
                if (!outfile.is_open())
                {
                    for(size_t i = 0; i < _filesUploaded.size(); i++)
                        remove(_filesUploaded[i].c_str());
                    throw (_errorCode = 500,_isError = true ,"return"); // Bad Request
                }
                outfile << _body.substr(0, _body.find(midBoundary));
                _currentLength += _body.substr(0, _body.find(midBoundary)).size();
                if(_currentLength > _location->max_body_size)
                {
                    for(size_t i = 0; i < _filesUploaded.size(); i++)
                        remove(_filesUploaded[i].c_str());
                    throw (_errorCode = 413 ,"return"); // Request Entity Too Large
                }
                outfile.close();
                _body.erase(0, _body.find(midBoundary));
                _isNotFirst = false;
            }
            else if(_body.find(endBoundary) != std::string::npos)
            {
                std::ofstream outfile(_fileupload.c_str(), std::ios::app | std::ios::binary );
                if (!outfile.is_open())
                {   
                    for(size_t i = 0; i < _filesUploaded.size(); i++)
                        remove(_filesUploaded[i].c_str());
                    throw (_errorCode = 500,_isError = true ,"return"); // Bad Request
                }
                outfile << _body.substr(0, _body.find(endBoundary));
                _currentLength += _body.substr(0, _body.find(endBoundary)).size();
                 if(_currentLength > _location->max_body_size)
                {
                    for(size_t i = 0; i < _filesUploaded.size(); i++)
                        remove(_filesUploaded[i].c_str());
                    throw (_errorCode = 413 ,"return"); // Request Entity Too Large
                }
                outfile.close();
                _body.erase(0, _body.find(endBoundary));
                _body.clear();
                isBodyEnd = true;
                throw(_errorCode = 201, _isError = true, "binary");
            }
            else if(_body.size() >= 100000)
            {
                std::ofstream outfile(_fileupload.c_str(), std::ios::app);
                if (!outfile.is_open())
                {
                    for(size_t i = 0; i < _filesUploaded.size(); i++)
                        remove(_filesUploaded[i].c_str());
                    throw (_errorCode = 501,_isError = true ,"return"); // Bad Request
                }
                outfile << _body.substr(0, _body.size() - 1000);
                _currentLength += _body.substr(0, _body.size() - 1000).size();
                 if(_currentLength > _location->max_body_size)
                {
                    for(size_t i = 0; i < _filesUploaded.size(); i++)
                        remove(_filesUploaded[i].c_str());
                    throw (_errorCode = 413 ,"return"); // Request Entity Too Large
                }
                outfile.close();
                _body.erase(0, _body.size() - 1000);
            }
            else
                break;
        }else
            break;
    }
}

void Client::readBoundryCgi(int fd)
{
    char buffer[3040];
    int ret = 1;
    if(_isreadFromClient == false)
    {
        ret = recv(fd, buffer, 3040, 0);
        if(ret <= 0)
            return ;
        timestamp = time(NULL);
        _body.append(buffer, ret);
        _isreadFromClient = true;
    }
    _boundry = _headers["Content-Type"].substr(_headers["Content-Type"].find("boundary=") + 9);
    if(_body.find("--"+_boundry +"--") != std::string::npos)
        isBodyEnd = true;
}


void Client::readChunkedCgi(int fd)
{
    char buffer[3040];
    int ret = 1;
    if(_isreadFromClient == false)
    {
        ret = recv(fd, buffer, 3040, 0);
        if(ret <= 0)
            return ;
        timestamp = time(NULL);
        _body.append(buffer, ret);
        _isreadFromClient = true;
    }
    if(_body.find("0\r\n\r\n") != std::string::npos)
        isBodyEnd = true;
}

void  Client::readContentLengthCgi(int fd)
{
    char buffer[3040];
    int ret = 1;
    if(_isreadFromClient == false)
    {
        ret = recv(fd, buffer, 3040, 0);
        if(ret <= 0)
            return ;
        timestamp = time(NULL);
        _body.append(buffer, ret);
        _isreadFromClient = true;
    }
    if(_body.length() == _contentLength)
        isBodyEnd = true;
    else if(_body.length() > _contentLength || _contentLength > _location->max_body_size)
        throw (_errorCode = 413 ,"return"); // Request Entity Too Large
}