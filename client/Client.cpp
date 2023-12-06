#include "Client.hpp"

Client::Client()
{
    _isparsed = false;
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
    char *buffer = new char[1024];
    int ret = 1;
    if (!_headerIsSend)
    {
        ret = recv(_socket, buffer, 1024, 0);
        if (ret <= 0)
            return (delete[] buffer, void());
        _request.append(buffer, ret);
    }
    if (_request.find("\r\n\r\n") == std::string::npos)
        return;
    ParseRequest(_request.substr(0, _request.find("\r\n\r\n") + 4));
    _body = _request.substr(_request.find("\r\n\r\n") + 4);
    checkRequest();
    uriToPath();
    _isparsed = true;
    delete[] buffer;
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
