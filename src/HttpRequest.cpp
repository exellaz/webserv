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

bool HttpRequest::parseHeaderLine(const std::string& line)
{
    size_t colon = line.find(':');
    if (colon != std::string::npos) {
        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + 1);
        while (!value.empty() && value[0] == ' ') {
            value = value.substr(1);
        }
        _headers[key] = value;
    }
    return true;
}