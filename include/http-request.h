#ifndef HTTP_REQUEST_H
# define HTTP_REQUEST_H

# include <iostream>
# include <sstream>
# include <map>
# include <cstdlib>

# include "http-exception.h"

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

        void clearRequest();
        bool hasHeader(const std::string& name) const;
        bool isHeaderParsed() const;
        bool isBodyParsed() const;

        const std::string& getMethod() const;
        const std::string& getURI() const;
        const std::string& getVersion() const;
        const std::map<std::string, std::string>& getHeaders() const;
        const std::string& getHeader(const std::string& name) const;
        const std::string& getBody() const;
        const std::string& getQueryString() const;
        const std::string& getSessionId() const;

        void setMethod(const std::string& method);
        void setURI(const std::string& uri);
        void setVersion(const std::string& version);
        void setHeader(const std::string& name, const std::string& value);
        void setBody(const std::string& body);
        void setQueryString(const std::string& str);
        void setSessionId(const std::string& id);

    private:
        bool _headerParsed;
        bool _bodyParsed;
        std::string _method;
        std::string _uri;
        std::string _version;
        std::map<std::string, std::string> _headers;
        std::string _body;
        std::string _queryString;
        std::string _sessionId;
};

std::ostream& operator<<(std::ostream &stream, const HttpRequest& src);

#endif
