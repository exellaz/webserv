#ifndef UTILS_H
# define UTILS_H

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <ctime>
#include <iomanip>

std::string toLower(const std::string& str);
bool isDigitsOnly(const std::string& str);
std::string getFullPath(const std::string &file);
std::string infoTime(void);
std::string toTitleCase(const std::string& str);

#endif
