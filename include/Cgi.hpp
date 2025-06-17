#ifndef CGIOBJECT_HPP
#define CGIOBJECT_HPP

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sys/wait.h>
#include "Configuration.hpp"
#include "http-request.h"
#include "http-response.h"

class Cgi
{
    public:
        ////process
        int                                 pipefd[2];
        int                                 status;
        pid_t                               pid;

        ////script
        std::string                         script_path; // Path to the CGI script
        char                                **argv; //argument vector for execve

        ////environment
        std::map<std::string, std::string>  env_vars; //set env
        std::vector<std::string>            env_str; //convert env to string
        char                                **envp; //convert string to char*

        ////method
        std::string executeCgi(HttpRequest &request, HttpResponse &response);
        Cgi();
        ~Cgi() {};
    private:
};

std::string getFullPath(const std::string &file);

//http response
void    handleCgiRequest(const std::string &output, HttpResponse &response);

#endif