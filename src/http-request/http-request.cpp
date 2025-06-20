#include "http-request.h"
#include "http-exception.h"

bool HttpRequest::parseRequestLine(const std::string& headerStr)
{
    std::istringstream stream(headerStr);
    std::string line;

    while (std::getline(stream, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }

        if (line.empty())
            continue; // Skip empty prelude lines

        // Now this is the actual request line
        std::string extra;
        std::istringstream lineStream(line);
        if (!(lineStream >> _method >> _uri >> _version))
            throw HttpException(BAD_REQUEST, "Bad request line format");

        if (_method != "GET" && _method != "POST" && _method != "DELETE")
            throw HttpException(METHOD_NOT_ALLOWED, "Method not allowed");

        if (_version != "HTTP/1.1")
            throw HttpException(VERSION_NOT_SUPPORTED, "HTTP version not supported");

        return true;
    }

    // If we never found a non-empty line
    throw HttpException(BAD_REQUEST, "Missing request line");
    return true;
}

bool HttpRequest::parseHeaderLines(const std::string& str)
{
    std::istringstream stream(str);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (line.empty())
            continue; // Skip empty prelude lines

        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            if (line[colon - 1] == ' ')
                throw HttpException(BAD_REQUEST, "Whitespace before colon");

            std::string key = line.substr(0, colon);
            // if ((_method == "GET" || _method == "DELETE") && (key == "Content-Length" || key == "Transfer-Encoding")) {
            //     response.setStatus(BAD_REQUEST);
            //     throw std::logic_error("Bad request");
            // }
            std::string value = line.substr(colon + 1);
            while (!value.empty() && value[0] == ' ')
                value = value.substr(1);

            if (_headers.count(key))
                throw HttpException(BAD_REQUEST, "Duplicate header");
            _headers[key] = value;
        }
    }

    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it) {
    			std::cout << "Header: [" << it->first << "] = [" << it->second << "]\n";
			}

    if (_headers.find("Host") == _headers.end())
        throw HttpException(BAD_REQUEST, "Missing Host header");
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

void HttpRequest::clearRequest()
{
    _method.clear();
    _uri.clear();
    _version.clear();
    _headers.clear();
    _body.clear();
}

std::ostream& operator<<(std::ostream &stream, const HttpRequest& src)
{
    stream << "Method: " << src.getMethod() << "\n";
    stream << "URI: " << src.getURI() << "\n";
    stream << "Version: " << src.getVersion() << "\n";
    stream << "\nHeaders\n";
    for (std::map<std::string, std::string>::const_iterator It = src.getHeaders().begin(); It != src.getHeaders().end(); ++It)
        stream << It->first << ": " << It->second << "\n";
    stream << "\nBody\n" << src.getBody() << "\n";
    return stream;
}
