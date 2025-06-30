#include "http-request.h"
#include "http-exception.h"

bool HttpRequest::isTChar(char c) const
{
    const std::string tcharSymbols = "!#$%&'*+-.^_`|~";
    return std::isalnum(static_cast<unsigned char>(c)) || tcharSymbols.find(c) != std::string::npos;
}

bool HttpRequest::isValidToken(const std::string& token) const
{
    for (std::string::const_iterator It = token.begin(); It != token.end(); ++It) {
        if (!isTChar(*It))
            return false;
    }
    return !token.empty();
}

bool HttpRequest::isValidHeaderValue(const std::string& value) const
{
    for (std::string::const_iterator It = value.begin(); It != value.end(); ++It) {
        unsigned char c = static_cast<unsigned char>(*It);
        if (c == 9 || (c >= 32 && c <= 126) || c >= 128)
            continue;

        return false;
    }
    return true;
}

void HttpRequest::parseRequestLine(const std::string& headerStr)
{
    std::istringstream stream(headerStr);
    std::string line;

    _headerParsed = true;
    while (std::getline(stream, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (line.empty())
            continue; // Skip empty prelude lines

        // Now this is the actual request line
        std::string extra;
        std::istringstream lineStream(line);
        if (!(lineStream >> _method >> _uri >> _version))
            throw HttpException(BAD_REQUEST, "Bad request line format");

        if (lineStream >> extra)
            throw HttpException(BAD_REQUEST, "Too many elements in request line");

        if (_uri.empty() || _uri[0] != '/')
            throw HttpException(BAD_REQUEST, "Invalid request-target");

        if (_method != "GET" && _method != "POST" && _method != "DELETE")
            throw HttpException(METHOD_NOT_ALLOWED, "Method not allowed");

        if (_version != "HTTP/1.1")
            throw HttpException(VERSION_NOT_SUPPORTED, "Only HTTP/1.1 supported");

        return ;
    }

    // If we never found a non-empty line
    throw HttpException(BAD_REQUEST, "Missing request line");
}

void HttpRequest::parseHeaderLines(const std::string& str)
{
    std::cout << "Method: " << _method << "\n";

    std::istringstream stream(str);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (line.empty())
            continue; // Skip empty prelude lines

        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            if (!isValidToken(key))
                throw HttpException(BAD_REQUEST, "Invalid token as header field");
            key = toLower(key);

            std::string value = line.substr(colon + 1);
            if (!isValidHeaderValue(value))
                throw HttpException(BAD_REQUEST, "Invalid header value");
            while (!value.empty() && value[0] == ' ')
                value = value.substr(1);

            if (_headers.count(key))
                throw HttpException(BAD_REQUEST, "Duplicate header");
            _headers[key] = value;
        }
    }

    if ((_method == "GET" || _method == "DELETE") && (hasHeader("Content-Length") || hasHeader("Transfer-Encoding")))
        throw HttpException(BAD_REQUEST, "No body expected for this method");

    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it)
        std::cout << "Header: [" << it->first << "] = [" << it->second << "]\n";

    if (!hasHeader("Host"))
        throw HttpException(BAD_REQUEST, "Missing Host header");

    if (hasHeader("Content-Length") && !isDigitsOnly(getHeader("Content-Length")))
        throw HttpException(BAD_REQUEST, "Invalid content length");
    _headerParsed = true;
}

void HttpRequest::clearRequest()
{
    _headerParsed = false;
    _bodyParsed = false;
    _method.clear();
    _uri.clear();
    _version.clear();
    _headers.clear();
    _body.clear();
}

HttpRequest::HttpRequest()
    : _headerParsed(false),
    _bodyParsed(false),
    _method(),
    _uri(),
    _version(),
    _headers(),
    _body()
{}

HttpRequest::HttpRequest(const HttpRequest& other)
    : _headerParsed(other._headerParsed),
    _bodyParsed(other._bodyParsed),
    _method(other._method),
    _uri(other._uri),
    _version(other._version),
    _headers(other._headers),
    _body(other._body)
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
    }
    return *this;
}

HttpRequest::~HttpRequest()
{}

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
