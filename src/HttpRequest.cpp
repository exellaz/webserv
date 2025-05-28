#include "HttpRequest.h"

bool HttpRequest::parseRequestLine(const std::string& line)
{
    std::istringstream lineStream(line);

    if (!(lineStream >> _method >> _url >> _version))
        return false;
    if (_method != "GET" && _method != "POST" && _method != "DELETE")
        return false; // This should return status 405 Method not allowed
    if (_version != "HTTP/1.1")
        return false; // This should return status 400 Bad Request
    return true;
}
