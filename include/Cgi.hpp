#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include "Configuration.hpp"
#include "http-request.h"
#include "http-response.h"

class Cgi
{
    private:
        int                                 pipefd[2];
        int                                 status;
        pid_t                               pid;
        std::string                         script_path;
        char                                **argv;
        std::map<std::string, std::string>  env_vars;
        std::vector<std::string>            env_str;
        char                                **envp;

    public:
        Cgi();
        ~Cgi() {};
        std::string executeCgi(HttpRequest &request, HttpResponse &response);

};

void    handleCgiRequest(const std::string &output, HttpResponse &response);

#endif