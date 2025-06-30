#include "http-response.h"
#include "http-request.h"
#include <sstream>
#include <cstdlib>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctime>

#include "sockets-polling.h"
// #include "Configuration.hpp"

HttpResponse::HttpResponse(StatusCode code)
    : _status(code)
{
    _headers["Server"] = "Webserv/1.0";
    _headers["Connection"] = "keep-alive";
}

HttpResponse::HttpResponse()
    : _status(OK)
{
    _headers["Server"] = "Webserv/1.0";
    _headers["Connection"] = "keep-alive";
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
    std::cout << "\r\n" << _body;
}

std::string HttpResponse::toString() {
    std::ostringstream responseStream;

    _headers["Date"] = getHttpDate();
    responseStream << buildStatusLine();
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
         it != _headers.end(); ++it) {
        responseStream << it->first << ": " << it->second << "\r\n";
    }
    responseStream << "\r\n";
    responseStream << _body;
    return responseStream.str();
}

void HttpResponse::clearResponse()
{
    _status = OK;
    _headers.clear();
    _headers["Server"] = "Webserv/1.0";
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

void HttpResponse::appendToBody(const std::string& bodyData)
{
    std::stringstream stream;

    _body.append(bodyData);
    stream << _body.size();
    setHeader("Content-Length", stream.str());
}

StatusCode HttpResponse::getStatus()
{
    return _status;
}

std::string HttpResponse::getHeader(const std::string& name)
{
    return _headers[name];
}

std::string HttpResponse::getBody()
{
    return _body;
}
