#include "http-response.h"
#include "http-request.h"
#include <sstream>
#include <cstdlib>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctime>

#include "sockets-polling.h"
#include "Configuration.hpp"

HttpResponse::HttpResponse(StatusCode code)
    : _status(code)
{
    _headers["Server"] = "Webserv/1.0";
}

HttpResponse::HttpResponse()
    : _status(OK)
{
    _headers["Server"] = "Webserv/1.0";
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

    // Add date header
    _headers["Date"] = getHttpDate();

    // Status line
    responseStream << buildStatusLine();

    // All headers
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
         it != _headers.end(); ++it) {
        responseStream << it->first << ": " << it->second << "\r\n";
    }

    responseStream << "\r\n";
    responseStream << _body;
    return responseStream.str();
}

// constructs the filesystem path given a docRoot and request path
std::string mapUriToPath(const std::string& docRoot, const std::string& uri) {
    std::string safeUri = uri;
    if (safeUri.empty() || safeUri[0] != '/')
        safeUri = "/" + safeUri;

    // Normalize “/” -> “/index.html” (i.e. default file)
    if (safeUri == "/")
        safeUri = "/index.html";

    std::cout << "Mapping URI: " << uri << " to path: " << docRoot + safeUri << "\n";
    return docRoot + safeUri;
}

// Reads the entire file at filepath into a std::string.
// Returns empty string if any error occurs.
std::string readFileToString(const std::string& filepath) {
    struct stat st;
    if (stat(filepath.c_str(), &st) < 0 || !S_ISREG(st.st_mode)) {
        return "";  // file doesn’t exist or isn’t regular
    }

    int fd = open(filepath.c_str(), O_RDONLY);
    if (fd < 0)
        return "";

    std::string content;
    content.reserve(st.st_size);

    const size_t BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    ssize_t n;
    while ((n = read(fd, buffer, BUFFER_SIZE)) > 0) {
        content.append(buffer, n);
    }
    close(fd);
    return content;
}


std::string getMimeType(const std::string& path)
{
    size_t dot = path.rfind('.');
    if (dot == std::string::npos)
        return "application/octet-stream";

    std::string ext = path.substr(dot + 1);
    if (ext == "html" || ext == "htm") return "text/html";
    if (ext == "css")                  return "text/css";
    if (ext == "js")                   return "application/javascript";
    if (ext == "png")                  return "image/png";
    if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
    if (ext == "gif")                  return "image/gif";
    if (ext == "txt")                  return "text/plain";
    // Fallback
    return "application/octet-stream";
}

void HttpResponse::handleGetRequest(const HttpRequest& request, Config &serverConfig)
{
    // Map URI to filesystem path
    //std::string fullPath = mapUriToPath(docRoot, request.getURI());
    std::string fullPath = resolveHttpPath(request, serverConfig);

    // Read from file
    std::string fileContents = readFileToString(fullPath);
    if (fileContents.empty()) {
        // If file not found or not readable -> 404 Not Found
        std::string body = "<html><body><h1>404 Not Found</h1></body></html>\n";
        setStatus(NOT_FOUND);
        setHeader("Content-Type", "text/html");
        setBody(body);
        return ;
    }

    // If file found -> 200 OK
    std::string mime = getMimeType(fullPath);
    setHeader("Content-Type", mime);
    setBody(fileContents);
}

// void HttpResponse::handlePostRequest(const HttpRequest& request)
// {

// }

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