#include <sys/stat.h>
#include <dirent.h> //for opendir, readdir, closedir
#include <fcntl.h>
#include <unistd.h>
#include "http-response.h"
#include "http-exception.h"
#include "Client.h"

static std::string readDirectorytoString(const std::string &directoryPath, const std::string& uri);

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

std::string validateIndex(const std::string& locationPath, const Location &location) {
    struct stat info;
    if (stat(locationPath.c_str(), &info) == 0 && S_ISDIR(info.st_mode) && (!location.getIndex().empty()))
        return (locationPath + "/" + location.getIndex());
    else
        return (locationPath);
}

void HttpResponse::handleGetRequest(const Location& location, const Client& client) {
    std::string fullPath = validateIndex(client.locationPath, location);

    struct stat info;
    if (stat(fullPath.c_str(), &info) < 0)
        throw HttpException(NOT_FOUND, "Path is not a file or directory");

    if (S_ISDIR(info.st_mode)) {
        std::string directoryContent = readDirectorytoString(fullPath, client.request.getURI());
        setStatus(OK);
        setHeader("Content-Type", "text/html");
        setBody(directoryContent);
        return ;
    }
    else if (S_ISREG(info.st_mode)) {
        std::string fileContents = readFileToString(fullPath);
        if (fileContents.empty())
            throw HttpException(NOT_FOUND, "File not found or readable");
        std::string mime = getMimeType(fullPath);
        setHeader("Content-Type", mime);
        setBody(fileContents);
        return ;
    }
}

/**
 * @brief Reads the contents of a directory and generates an HTML index page
*/
static std::string readDirectorytoString(const std::string &directoryPath, const std::string& uri) {
    std::stringstream htmlOutput;
    htmlOutput << "<html><head><title>Index of " << uri << "</title></head><body>";
    htmlOutput << "<h1>Index of " << uri << "</h1><hr><ul>";
    DIR* dir = opendir(directoryPath.c_str());
    if (!dir)
        return "";
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name == ".")
            continue;
        std::string slash = (entry->d_type == DT_DIR) ? "/" : "";
        std::string url_base = uri;
        if (url_base.empty() || url_base[url_base.size() - 1] != '/')
            url_base += '/';
        htmlOutput << "<li><a href=\"" << url_base << name << slash << "\">" << name << slash << "</a></li>";
    }
    closedir(dir);
    htmlOutput << "</ul><hr></body></html>";
    return htmlOutput.str();
}