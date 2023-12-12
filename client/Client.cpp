#include "Client.hpp"

Client::Client() : Request(), Response()
{
    _isparsed = false;
    _isReadBody = false;
    _server = NULL;
    _servers = NULL;
}
Client::~Client()
{
}

void Client::readRequest()
{
    if (_isparsed == false)
        readheader();
    else if (_isReadBody == true)
        readbody();
}

void Client::readheader()
{
    char buffer[1024];
    int ret = 1;
    if (!_headerIsRecv)
    {
        ret = recv(_socket, buffer, 1024, 0);
        if (ret <= 0)
            return;
        _request.append(buffer, ret);
    }
    timestamp = time(NULL);
    if (_request.find("\r\n\r\n") == std::string::npos)
        return;
    std::cout << _request << std::endl;
    ParseRequest(_request.substr(0, _request.find("\r\n\r\n") + 4));
    _server = &findServer();
    _body = _request.substr(_request.find("\r\n\r\n") + 4);
    checkRequest();
    uriToPath();
    _headersRequest = _headers;
    matchlocation();
    if (getHeader("Content-Length") != NULL)
    {
        std::stringstream ss;
        ss << getHeader("Content-Length")->c_str();
        ss >> _contentLength;
    }
    _file = _pathFile;
    _isparsed = true;
}

void Client::readbody()
{
    if (isBodyEnd == false)
    {
        if (getHeader("Transfer-Encoding") != NULL)
            readChunked(_socket);
        else if (getHeader("Content-Type") != NULL && getHeader("Content-Type")->find("boundary=") != std::string::npos)
        {
            readBoundry(_socket);
        }
        else
            readContentLength(_socket);
    }
    if(isBodyEnd == true)
    {
        parseRequestBody();
    }
}

void Client::sendResponse()
{
    signal(SIGPIPE, SIG_IGN);
    std::string *tmp = NULL;
    size_t pos = 0;
    if (_errorCode == 0)
        _errorCode = 200;
    _statusCode = _errorCode;
    if ((tmp = getHeader("Range")))
    {
        std::stringstream ss;
        *tmp = tmp->substr(tmp->find("=") + 1);
        ss << *tmp;
        ss >> pos;
        std::cout << "start pos : " << pos << std::endl;
        sendRangeBody(_socket, pos);
    }
    else
    {
        if (_isheadSend == false)
            sendHeaders(_socket);
        else if (isBodyString == true)
            sendBodyString(_socket);
        else
            sendBody(_socket);
    }
    timestamp = time(NULL);
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
        for (size_t j = 0; j < tmpServer[i]->getServerNames().size(); j++)
            if (tmpServer[i]->getServerNames()[j] == tmp_host)
                return *tmpServer[i];
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
            {
                _headersResponse["Location"] = "http://" + _headers["Host"] + _location->_return.second;
            }
            else
            {
                _headersResponse["Location"] = _location->_return.second;
            }
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



void Client::parseRequestBody()
{
    std::cout << "parseRequestBody" << std::endl;
    // request is multipart/form-data or binary
    if(_headers.find("Content-Length") != _headers.end() \
    && _headers.find("Transfer-Encoding") == _headers.end())
    {
        if (_headers.find("Content-Type") != _headers.end() \
        && _headers["Content-Type"].find("boundary=") != std::string::npos)
            parseMultipartData();

        else
            parseBinaryData();

    }
    // request is chunked
    else if(_headers.find("Transfer-Encoding") != _headers.end() \
    && _headers["Transfer-Encoding"].find("chunked") != std::string::npos)
        parseChunkedData();
}

void Client::createFile(std::string path)
{
    std::ofstream file;
    file.open(path.c_str(), std::ios::binary);
    if(!file.is_open())
        throw (_errorCode = 501, "file open error");
    file << _body;
    file.close();
}

char randchar()
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    return alphanum[rand() % (sizeof(alphanum) - 1)];
}

std::string Client::generateRandomString ()
{
    size_t length = 10;
    srand(time(NULL));
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

void Client::parseMultipartData()
{
    std::string midBoundary = "--"  + _boundry + "\r\n";
    std::string endBoundary = "--"  + _boundry + "--\r\n";
    std::string tmp = _body.substr(0, _body.find("\r\n\r\n"));
    _body.erase(0, _body.find("\r\n\r\n") + 4);

    filename = tmp.substr(tmp.find("filename=") + 10);
    filename = filename.substr(0, filename.find("\""));
    std::string path = _location->root;
    if(path.back() != '/')
        path += "/";
    path += filename;

    while (_body.find(_boundry) != std::string::npos)
    {
        if(_body.find(midBoundary) != std::string::npos)
            _body.erase( _body.find(midBoundary) , midBoundary.length());
        else if(_body.find(endBoundary) != std::string::npos)
            _body.erase( _body.find(endBoundary) , endBoundary.length());
    }
    createFile(path);
    throw (_errorCode = 201,_isError = true,"good shit");
}

void Client::parseBinaryData()
{
    std::string path = _location->root;
    filename = generateRandomString();
    if(path.back() != '/')
        path += '/';
    path += filename;
    createFile(path);
    throw (_errorCode = 201,_isError = true,"binary");
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

void Client::parseChunkedData()
{
    std::string path;
    _body = unchunk (_body);
    struct stat filehelp;
    stat(_pathFile.c_str(), &filehelp);
    if (S_ISDIR(filehelp.st_mode))  
    {
        path = _pathFile ;
        if (path.back() != '/')
            path += '/';
        filename = generateRandomString();
        path += filename;
    }
    else
        path = _pathFile; 
    createFile (path);
    throw (_errorCode = 201,_isError = true,"unchunked");
}
