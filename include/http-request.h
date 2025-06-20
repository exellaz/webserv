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
        bool parseRequestLine(const std::string& headerStr);
        bool parseHeaderLines(const std::string& line);
        bool parseRequestBody(const std::string& str);
        void clearRequest();

        bool hasHeader(const std::string& name) const;
        bool isValidToken(const std::string& token) const;
        bool isTChar(char c) const;
        bool isValidHeaderValue(const std::string& value) const;


        const std::string& getMethod() const;
        const std::string& getURI() const;
        const std::string& getVersion() const;
        const std::map<std::string, std::string>& getHeaders() const;
        const std::string& getHeader(const std::string& name) const;
        const std::string& getBody() const;

        void setMethod(const std::string& method);
        void setURI(const std::string& uri);
        void setVersion(const std::string& version);
        void setHeader(const std::string& name, const std::string& value);
        void setBody(const std::string& body);

    private:
        std::string _method;
        std::string _uri;
        std::string _version;
        std::map<std::string, std::string> _headers;
        std::string _body;

};

std::ostream& operator<<(std::ostream &stream, const HttpRequest& src);

#endif
