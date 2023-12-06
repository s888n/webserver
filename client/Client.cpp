#include "Client.hpp"

Client::Client()
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
    if (!_headerIsSend)
    {
        ret = recv(_socket, buffer, 1024, 0);
        if (ret <= 0)
            return ;
        _request.append(buffer, ret);
    }
    if (_request.find("\r\n\r\n") == std::string::npos)
        return;
    ParseRequest(_request.substr(0, _request.find("\r\n\r\n") + 4));
    _server = &findServer();
    _body = _request.substr(_request.find("\r\n\r\n") + 4);
    checkRequest();
    uriToPath();
    
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
    std::string *tmp = NULL;
    size_t pos = 0;
    if ((tmp = getHeader("Range")))
    {
        std::stringstream ss;
        tmp->substr(tmp->find("=") + 1);
        ss << *tmp;
        ss >> pos;
        sendRangeBody(_socket, pos);
    }
    else
    {
        if (_isheadSend == false)
            sendHeaders(_socket);
        else
            sendBody(_socket);
    }
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
        if(tmpServer[i]->getHost() == tmp_host)
            return *tmpServer[i];
    return *tmpServer[0];
}
