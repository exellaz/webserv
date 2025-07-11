#include "http-response.h"

HttpResponse::HttpResponse()
    : _status(OK)
{
    setHeader("Server", "Webserv/1.0");
    setHeader("Connection", "keep-alive");
}

HttpResponse::HttpResponse(StatusCode code)
    : _status(code)
{
    setHeader("Server", "Webserv/1.0");
    setHeader("Connection", "keep-alive");
}

HttpResponse::HttpResponse(const HttpResponse& other)
    : _status(other._status),
    _headers(other._headers),
    _body(other._body)
{}

HttpResponse& HttpResponse::operator=(const HttpResponse& other)
{
    if (this != &other) {
        _status = other._status;
        _headers = other._headers;
        _body = other._body;
    }
    return *this;
}

HttpResponse::~HttpResponse()
{}

std::string HttpResponse::reasonPhrase(StatusCode code) const
{
    switch (code) {
        case OK:                     return "OK";
        case CREATED:                return "Created";
        case NO_CONTENT:             return "No Content";
        case MOVED_PERMANENTLY:      return "Moved Permanently";
        case FOUND:                  return "Found";
        case BAD_REQUEST:            return "Bad Request";
        case UNAUTHORIZED:           return "Unauthorized";
        case FORBIDDEN:              return "Forbidden";
        case NOT_FOUND:              return "Not Found";
        case METHOD_NOT_ALLOWED:     return "Method Not Allowed";
        case PAYLOAD_TOO_LARGE:      return "Payload Too Large";
        case INTERNAL_ERROR:         return "Internal Server Error";
        case NOT_IMPLEMENTED:        return "Not Implemented";
        case VERSION_NOT_SUPPORTED:  return "HTTP Version Not Supported";
    }
    return "Unknown";
}

std::string HttpResponse::buildStatusLine() const
{
    std::ostringstream line;
    line << "HTTP/1.1 "
         << static_cast<int>(_status) << " "
         << reasonPhrase(_status)
         << "\r\n";
    return line.str();
}

void HttpResponse::printResponseHeaders()
{
    for (std::map<std::string, std::string>::iterator It = _headers.begin(); It != _headers.end(); ++It) {
        std::cout << It->first << ": " << It->second << "\r\n";
    }
    std::cout << "\r\n" << _body;
}

void HttpResponse::clearResponse()
{
    _status = OK;
    _headers.clear();
    setHeader("Server", "Webserv/1.0");
    setHeader("Connection", "keep-alive");
    _body.clear();
}

std::string HttpResponse::getHttpDate()
{
    time_t now = time(NULL);
    struct tm gm;

    gmtime_r(&now, &gm);

    char buf[64];
    // Format: "Tue, 15 Nov 1994 08:12:31 GMT"
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &gm);
    return std::string(buf);
}

std::string HttpResponse::toString()
{
    std::ostringstream responseStream;

    setHeader("Date", getHttpDate());
    responseStream << buildStatusLine();
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
         it != _headers.end(); ++it) {
        responseStream << toTitleCase(it->first) << ": " << it->second << "\r\n";
    }
    responseStream << "\r\n";
    responseStream << _body;
    return responseStream.str();
}

std::ostream& operator<<(std::ostream &stream, const HttpResponse& src)
{
    std::string timePrefix = infoTime();

    std::cout << infoTime() << BOLD ORANGE "=== HTTP RESPONSE BEGIN ===" RESET << "\n";

    stream << infoTime() << src.buildStatusLine();
    for (std::map<std::string, std::string>::const_iterator it = src.getHeaders().begin();
         it != src.getHeaders().end(); ++it) {
        stream << infoTime() << toTitleCase(it->first) << ": " << it->second << "\r\n";
    }

    std::cout << infoTime() << BOLD ORANGE "=== HTTP RESPONSE END ===" RESET << "\n";
    return stream;
}