#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sys/wait.h>

#include "utils.h"
#include "http-request.h"

class Cgi
{
    private:
        int                                 pipefd[2];
        int                                 status;
        pid_t                               pid;
        std::string                         scriptPath;
        char                                **argv;
        std::map<std::string, std::string>  envVars;
        std::vector<std::string>            envStr;
        char                                **envp;

    public:
        Cgi();
        ~Cgi() {};

        std::string executeCgi(HttpRequest &request, HttpResponse &response);
};

void    handleCgiRequest(const std::string &output, HttpResponse &response);

#endif
