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
        HttpRequest();
        HttpRequest(const HttpRequest& other);
        HttpRequest& operator=(const HttpRequest& other);
        ~HttpRequest();

        void parseRequestLine(const std::string& headerStr);
        void parseHeaderLines(const std::string& line);
        void extractQueryString();
        std::string generateSessionId();
        std::string getSessionIdFromCookie(const std::string& cookieHeader);


        void clearRequest();
        bool hasHeader(const std::string& name) const;
        bool isValidToken(const std::string& token) const;
        bool isTChar(char c) const;
        bool isValidHeaderValue(const std::string& value) const;
        bool isHeaderParsed() const;
        bool isBodyParsed() const;

        const std::string& getMethod() const;
        const std::string& getURI() const;
        const std::string& getVersion() const;
        const std::map<std::string, std::string>& getHeaders() const;
        const std::string& getHeader(const std::string& name) const;
        const std::string& getBody() const;
        const std::string& getQueryString() const;

        void setMethod(const std::string& method);
        void setURI(const std::string& uri);
        void setVersion(const std::string& version);
        void setHeader(const std::string& name, const std::string& value);
        void setBody(const std::string& body);
        void setQueryString(const std::string& str);

    private:
        bool _headerParsed;
        bool _bodyParsed;
        std::string _method;
        std::string _uri;
        std::string _version;
        std::map<std::string, std::string> _headers;
        std::string _body;
        std::string _queryString;
};

std::ostream& operator<<(std::ostream &stream, const HttpRequest& src);

#endif
