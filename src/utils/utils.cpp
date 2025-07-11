#include "utils.h"
#include "http-exception.h"
#include "color.h"

std::string toLower(const std::string& str)
{
    std::string result = str;
    for (size_t i = 0; i < str.size(); ++i)
        result[i] = std::tolower(static_cast<int>(result[i]));
    return result;
}

bool isDigitsOnly(const std::string& str)
{
    for (size_t i = 0; i < str.size(); ++i) {
        if (!std::isdigit(static_cast<int>(str[i])))
            return false;
    }
    return true;
}

/**
 * @brief get full path of the file
 * @param file name of the file
 * @return full path of the file
*/
std::string getFullPath(const std::string &file)
{
    std::string full_path;
    char *cwd = getcwd(NULL, 0);
    if (!cwd) {
        perror("getcwd");
        return "";
    }
    full_path = std::string(cwd) + file;
    free(cwd);
    return full_path;
}

std::string infoTime()
{
    std::time_t now = std::time(NULL);
    std::tm *ltm = std::localtime(&now);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", ltm);
    return std::string(BOLD CYAN "[ ") + buf + " ] " RESET;
}

std::string toTitleCase(const std::string& str)
{
    std::string result;
    bool capitalizeNext = true;

    for (std::size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        if (c == '-') {
            result += c;
            capitalizeNext = true;
        }
        else if (capitalizeNext) {
            result += std::toupper(c);
            capitalizeNext = false;
        }
        else {
            result += std::tolower(c);
        }
    }

    return result;
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
    if (ext == "json")                 return "text/plain";
    if (ext == "cgi")                  return "text/plain";
    if (ext == "py")                   return "text/plain";
    return "application/octet-stream";
}

std::string readFileToString(const std::string& filepath)
{
    struct stat st;
    if (stat(filepath.c_str(), &st) < 0 || !S_ISREG(st.st_mode))
        throw HttpException(NOT_FOUND, "File does not exist or is not regular");

    int fd = open(filepath.c_str(), O_RDONLY);
    if (fd < 0)
        throw HttpException(NOT_FOUND, "File cannot be open");

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