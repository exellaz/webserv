#include "http-request.h"

bool HttpRequest::isHeaderParsed() const
{
    return _headerParsed;
}

bool HttpRequest::isBodyParsed() const
{
    return _bodyParsed;
}

bool HttpRequest::hasHeader(const std::string& name) const
{
    return _headers.find(toLower(name)) != _headers.end();
}

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

const std::string& HttpRequest::getQueryString() const
{
    return _queryString;
}

const std::string& HttpRequest::getHeader(const std::string& name) const
{
    static const std::string empty;
    std::map<std::string, std::string>::const_iterator it = _headers.find(toLower(name));
    if (it != _headers.end())
        return it->second;
    return empty;
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

void HttpRequest::setQueryString(const std::string& str)
{
    _queryString = str;
}
