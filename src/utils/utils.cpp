#include "utils.h"

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

