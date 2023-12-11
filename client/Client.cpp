#include "Client.hpp"

Client::Client(): Request(), Response()
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
            return ;
        _request.append(buffer, ret);
    }
    timestamp = time(NULL);
    if (_request.find("\r\n\r\n") == std::string::npos)
        return;
    ParseRequest(_request.substr(0, _request.find("\r\n\r\n") + 4));
    _server = &findServer();
    _body = _request.substr(_request.find("\r\n\r\n") + 4);
    checkRequest();
    uriToPath();
    _headersRequest = _headers;
    matchlocation();
    _file = _pathFile;
    _isparsed = true;
}

void Client::readbody()
{
    char *buffer = new char[1024];
    int ret = 1;
    ret = recv(_socket, buffer, 1024, 0);
    if (ret <= 0)
        return (delete[] buffer, void());
    _body.append(buffer, ret);
    delete[] buffer;
}

void Client::sendResponse()
{
    signal(SIGPIPE, SIG_IGN);
    std::string *tmp = NULL;
    size_t pos = 0;
    if(_errorCode == 0)
        _errorCode = 200;
    _statusCode = _errorCode;
    if ((tmp = getHeader("Range")))
    {
        std::stringstream ss;
        *tmp =  tmp->substr(tmp->find("=") +  1);
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

server& Client::findServer()
{
    std::vector<server*> tmpServer;
    std::string tmp_host;
    std::string tmp_port;
    std::string *_host = getHeader("Host");
    if(_host != NULL)
    {
        tmp_host = _host->substr(0, _host->find(":"));
        tmp_port = _host->substr(_host->find(":") + 1);
    }
    for(size_t i = 0; i < _servers->size(); i++)
        if(std::to_string(_servers->at(i).getPort()) == tmp_port || _servers->at(i).fd == serverFd)
            tmpServer.push_back(&_servers->at(i));
    for(size_t i = 0; i < tmpServer.size(); i++)
        for(size_t j = 0; j < tmpServer[i]->getServerNames().size(); j++)
            if(tmpServer[i]->getServerNames()[j] == tmp_host)
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
    if( _location == NULL ||_location->isReturn == false)
        return false;
    if(_location->_return.first != -1 && _location->_return.second != "")
    {
        if(std::find(numbers.begin(), numbers.end(), _location->_return.first) != numbers.end())
        {
                _errorCode = _location->_return.first;
            if(_location->_return.second.front() == '/' && _headers.find("Host") != _headers.end())
            {
                _headersResponse["Location"] = "http://"+_headers["Host"]  + _location->_return.second;
            }
            else
            {
                _headersResponse["Location"] = _location->_return.second;

            }
        }else
        {
            _errorCode = _location->_return.first;
            _body = _location->_return.second;
        }
    }
    else if(_location->_return.first != -1 && _location->_return.second == "")
    {
        _errorCode = _location->_return.first;
    }
    else if(_location->_return.first == -1 && _location->_return.second != "")
    {
        _headersResponse["Location"] = _location->_return.second;
        _errorCode = 301;
    }
    return true;
 }
