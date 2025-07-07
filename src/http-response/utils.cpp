#include "http-response.h"

StatusCode HttpResponse::getStatus()
{
    return _status;
}

std::string HttpResponse::getHeader(const std::string& name)
{
    static const std::string empty;
    std::map<std::string, std::string>::const_iterator it = _headers.find(toLower(name));
    if (it != _headers.end())
        return it->second;
    return empty;
}

std::string HttpResponse::getBody()
{
    return _body;
}

void HttpResponse::setStatus(StatusCode code)
{
    _status = code;
}

void HttpResponse::setHeader(const std::string& name, const std::string& value)
{
    _headers[toLower(name)] = value;
}

void HttpResponse::setBody(const std::string& bodyData)
{
    std::stringstream stream;

    _body = bodyData;
    stream << _body.size();
    setHeader("Content-Length", stream.str());
}

void HttpResponse::appendToBody(const std::string& bodyData)
{
    std::stringstream stream;

    _body.append(bodyData);
    stream << _body.size();
    setHeader("Content-Length", stream.str());
}
