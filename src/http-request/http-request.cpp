#include "http-request.h"

bool HttpRequest::parseRequestLine(const std::string& line)
{
    std::istringstream lineStream(line);

    if (!(lineStream >> _method >> _uri >> _version))
        return false;
    if (_method != "GET" && _method != "POST" && _method != "DELETE")
        return false; // This should return status 405 Method not allowed
    if (_version != "HTTP/1.1")
        return false; // This should return status 400 Bad Request
    return true;
}

bool HttpRequest::parseHeaderLines(const std::string& str, HttpResponse& response)
{
    std::istringstream stream(str);
    std::string line;
    while (std::getline(stream, line) && line != "\r") {
        if (!line.empty() && line[line.length() - 1] == '\r') {
            line = line.substr(0, line.length() - 1);
        }
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            if ((_method == "GET" || _method == "DELETE") && (key == "Content-Length" || key == "Transfer-Encoding"))
                response.setStatus(BAD_REQUEST);
            std::string value = line.substr(colon + 1);
            while (!value.empty() && value[0] == ' ') {
                value = value.substr(1);
            }
            _headers[key] = value;
        }
    }
    return true;
}

bool HttpRequest::parseRequestBody(const std::string& str)
{
    size_t headerEnd = str.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return false;

    // Compute where the body starts
    size_t bodyStart = headerEnd + 4;  // skip “\r\n\r\n”
    // See if there’s a Content-Length header
    std::string clStr = _headers["Content-Length"];
    if (!clStr.empty()) {
        int content_length = std::atoi(clStr.c_str());
        // Don’t read past the end of str
        size_t maxAvailable = str.size() - bodyStart;
        size_t toCopy = std::min<size_t>(content_length, maxAvailable);

        _body = str.substr(bodyStart, toCopy);
    }
    return true;
}

std::ostream& operator<<(std::ostream &stream, const HttpRequest& src)
{
    stream << "Method: " << src._method << "\n";
    stream << "URI: " << src._uri << "\n";
    stream << "Version: " << src._version << "\n";
    stream << "\nHeaders\n";
    for (std::map<std::string, std::string>::const_iterator It = src._headers.begin(); It != src._headers.end(); ++It)
        stream << It->first << ": " << It->second << "\n";
    stream << "\nBody\n" << src._body << "\n";
    return stream;
}
