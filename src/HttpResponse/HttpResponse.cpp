#include "HttpResponse.h"
#include <sstream>
#include <cstdlib>

HttpResponse::HttpResponse(StatusCode code)
    : _status(code)
{
}

std::string HttpResponse::reasonPhrase(StatusCode code)
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

std::string HttpResponse::buildStatusLine()
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
}

void HttpResponse::setStatus(StatusCode code)
{
    _status = code;
}

void HttpResponse::setHeader(const std::string& name, const std::string& value)
{
    _headers[name] = value;
}

void HttpResponse::setBody(const std::string& bodyData)
{
    std::stringstream stream;

    _body = bodyData;
    stream << _body.size();
    setHeader("Content-Length", stream.str());
}
