#include "http-request.h"

static bool isTChar(char c);
static bool isValidToken(const std::string& token);
static bool isValidHeaderValue(const std::string& value);

void HttpRequest::parseRequestLine(const std::string& headerStr)
{
    std::istringstream stream(headerStr);
    std::string line;

    _headerParsed = true;
    while (std::getline(stream, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (line.empty())
            continue;

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

        extractQueryString();
        return ;
    }

    throw HttpException(BAD_REQUEST, "Missing request line");
}

void HttpRequest::parseHeaderLines(const std::string& str)
{
    std::istringstream stream(str);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (line.empty())
            continue;

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

    if (!hasHeader("Host"))
        throw HttpException(BAD_REQUEST, "Missing Host header");

    if (hasHeader("Content-Length") && !isDigitsOnly(getHeader("Content-Length")))
        throw HttpException(BAD_REQUEST, "Invalid content length");
    _headerParsed = true;
}

void HttpRequest::extractQueryString()
{
    size_t qpos = _uri.find('?');
    if (qpos != std::string::npos) {
        _queryString = _uri.substr(qpos + 1);
        _uri = _uri.substr(0, qpos);
    }
    else
        _queryString.clear();
}

std::ostream& operator<<(std::ostream &stream, const HttpRequest& src)
{
    stream << "Method: " << src.getMethod() << "\n";
    stream << "URI: " << src.getURI() << "\n";
    stream << "Query String: " << src.getQueryString() << "\n";
    stream << "Version: " << src.getVersion() << "\n\n";
    for (std::map<std::string, std::string>::const_iterator it = src.getHeaders().begin(); it != src.getHeaders().end(); ++it)
        stream << "Header: [" << it->first << "] = [" << it->second << "]\n";
    stream << std::endl;
    if (!src.getBody().empty())
        stream << "Body\n" << src.getBody() << "\n";
    return stream;
}

static bool isTChar(char c)
{
    const std::string tcharSymbols = "!#$%&'*+-.^_`|~";
    return std::isalnum(static_cast<unsigned char>(c)) || tcharSymbols.find(c) != std::string::npos;
}

static bool isValidToken(const std::string& token)
{
    for (std::string::const_iterator It = token.begin(); It != token.end(); ++It) {
        if (!isTChar(*It))
            return false;
    }
    return !token.empty();
}

static bool isValidHeaderValue(const std::string& value)
{
    for (std::string::const_iterator It = value.begin(); It != value.end(); ++It) {
        unsigned char c = static_cast<unsigned char>(*It);
        if (c == 9 || (c >= 32 && c <= 126) || c >= 128)
            continue;

        return false;
    }
    return true;
}
