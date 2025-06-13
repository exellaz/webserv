#include "http-request.h"

const std::string& HttpRequest::getMethod() const
{
    return _method;
}

const std::string& HttpRequest::getURI() const
{
    return _uri;
}

const std::string& HttpRequest::getVersion() const
{
    return _version;
}

const std::map<std::string, std::string>& HttpRequest::getHeaders() const
{
    return _headers;
}

const std::string& HttpRequest::getHeader(const std::string& name) const
{
    return _headers.at(name);
}

const std::string& HttpRequest::getBody() const
{
    return _body;
}

void HttpRequest::setMethod(const std::string& method)
{
    _method = method;
}

void HttpRequest::setURI(const std::string& uri)
{
    _uri = uri;
}

void HttpRequest::setVersion(const std::string& version)
{
    _version = version;
}

void HttpRequest::setHeader(const std::string& name, const std::string& value)
{
    _headers[name] = value;
}

void HttpRequest::setBody(const std::string& body)
{
    _body = body;
}