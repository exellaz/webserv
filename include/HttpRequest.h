#ifndef HTTP_REQUEST_H
# define HTTP_REQUEST_H

# include <iostream>
# include <sstream>
# include <map>

class HttpRequest
{
    public:
        bool parseRequestLine(const std::string& line);

        std::string _method;
        std::string _url;
        std::string _version;
        std::map<std::string, std::string> _headers;
        std::string _body;

    private:

};

#endif