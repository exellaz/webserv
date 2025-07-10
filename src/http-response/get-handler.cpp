#include <dirent.h>
#include <unistd.h>
#include "http-response.h"
#include "http-exception.h"
#include "Client.h"
#include "utils.h"

static std::string readDirectorytoString(const std::string &directoryPath, const std::string& uri);

std::string validateIndex(const std::string& locationPath, const Location &location) {
    struct stat info;
    if (stat(locationPath.c_str(), &info) == 0 && S_ISDIR(info.st_mode) && (!location.getIndex().empty()))
        return (locationPath + "/" + location.getIndex());
    else
        return (locationPath);
}

void HttpResponse::handleGetRequest(const Location& location, const Client& client) {
    std::string fullPath = validateIndex(client.getLocationPath(), location);

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
static std::string readDirectorytoString(const std::string &directoryPath, const std::string& uri)
{
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
