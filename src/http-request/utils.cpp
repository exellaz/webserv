#include "http-request.h"
#include "utils.h"

HttpRequest::HttpRequest()
    : _headerParsed(false),
    _bodyParsed(false),
    _method(),
    _uri(),
    _version(),
    _headers(),
    _body(),
    _queryString()
{}

HttpRequest::HttpRequest(const HttpRequest& other)
    : _headerParsed(other._headerParsed),
    _bodyParsed(other._bodyParsed),
    _method(other._method),
    _uri(other._uri),
    _version(other._version),
    _headers(other._headers),
    _body(other._body),
    _queryString(other._queryString)
{}

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
    if (this != &other) {
        _headerParsed = other._headerParsed;
        _bodyParsed = other._bodyParsed;
        _method = other._method;
        _uri = other._uri;
        _version = other._version;
        _headers = other._headers;
        _body = other._body;
        _queryString = other._queryString;
    }
    return *this;
}

HttpRequest::~HttpRequest()
{}

void HttpRequest::clearRequest()
{
    _headerParsed = false;
    _bodyParsed = false;
    _method.clear();
    _uri.clear();
    _version.clear();
    _headers.clear();
    _body.clear();
    _queryString.clear();
}

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

const std::string& HttpRequest::getSessionId() const
{
    return _sessionId;
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
    _headers[toLower(name)] = value;
}

void HttpRequest::setBody(const std::string& body)
{
    _body = body;
}

void HttpRequest::setQueryString(const std::string& str)
{
    _queryString = str;
}

void HttpRequest::setSessionId(const std::string& id)
{
    _sessionId = id;
}
