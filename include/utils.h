#ifndef UTILS_H
# define UTILS_H

# include <iostream>
#include <sys/wait.h>

std::string toLower(const std::string& str);
bool isDigitsOnly(const std::string& str);
std::string getFullPath(const std::string &file);


#endif