#ifndef HTTP_REQUEST_H
# define HTTP_REQUEST_H

# include <iostream>
# include <sstream>
# include <map>
# include <cstdlib>


namespace HttpConstants
{
    // Supported Methods
    const std::string GET = "GET";
    const std::string POST = "POST";
    const std::string DELETE = "DELETE";

    const std::string HTTP_1_1 = "HTTP/1.1";
}

class HttpRequest
{
    public:
        bool parseRequestLine(const std::string& line);
        bool parseHeaderLines(const std::string& line);
        bool parseRequestBody(const std::string& str);


        std::string _method;
        std::string _url;
        std::string _version;
        std::map<std::string, std::string> _headers;
        std::string _body;

    private:

};

std::ostream& operator<<(std::ostream &stream, const HttpRequest& src);

#endif