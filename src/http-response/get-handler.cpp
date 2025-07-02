#include "http-response.h"
#include "http-exception.h"
#include "sockets-polling.h"

// Reads the entire file at filepath into a std::string.
// Returns empty string if any error occurs.
static std::string readFileToString(const std::string& filepath) {
    struct stat st;
    if (stat(filepath.c_str(), &st) < 0 || !S_ISREG(st.st_mode)) {
        return "";
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

static std::string getMimeType(const std::string& path)
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
    if (ext == "json")                 return "text/plain";
    if (ext == "cgi")                  return "text/plain";
    if (ext == "py")                   return "text/plain";
    // Fallback
    return "application/octet-stream";
}

std::string validateIndex(const std::string& locationPath, const Location &location)
{
    struct stat info;
    if (stat(locationPath.c_str(), &info) == 0 && S_ISDIR(info.st_mode) && (!location.getIndex().empty()))
    {
        std::cout << GREEN "URI AFTER: " << locationPath + "/" + location.getIndex() << "\n" RESET; //// debug
        return (locationPath + "/" + location.getIndex());
    }
    else
    {
        std::cout << BLUE "URI AFTER: " << locationPath << "\n" RESET;
        return (locationPath);
    }
}

void HttpResponse::handleGetRequest(const Location& location, const Client& client)
{
    std::string fullPath = validateIndex(client.locationPath, location);

    struct stat info;
    if (stat(fullPath.c_str(), &info) < 0)
        throw HttpException(NOT_FOUND, "Path is not a file or directory");

    if (S_ISDIR(info.st_mode)) {
        std::cout << GREEN "AutoIndex found\n" RESET; //// debug
        std::string directoryContent = readDirectorytoString(fullPath, client.request.getURI());
        setStatus(OK);
        setHeader("Content-Type", "text/html");
        setBody(directoryContent);
        return ;
    }
    else if (S_ISREG(info.st_mode))
    {
        std::cout << GREEN "Static file found\n" RESET; //// debug
        std::string fileContents = readFileToString(fullPath);
        if (fileContents.empty())
            throw HttpException(NOT_FOUND, "File not found or readable");
        std::string mime = getMimeType(fullPath);
        setHeader("Content-Type", mime);
        setBody(fileContents);
        return ;
    }
}
