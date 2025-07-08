#include "utils.h"

std::string toLower(const std::string& str) {
    std::string result = str;
    for (size_t i = 0; i < str.size(); ++i)
        result[i] = std::tolower(static_cast<int>(result[i]));
    return result;
}

bool isDigitsOnly(const std::string& str) {
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
std::string getFullPath(const std::string &file) {
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