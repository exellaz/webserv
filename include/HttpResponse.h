#ifndef HTTP_RESPONSE_H
# define HTTP_RESPONSE_H

# include <iostream>
# include <map>
// # include "HttpRequest.h"

class HttpRequest;

namespace HttpCodes {
    enum StatusCode {
        OK = 200,
        CREATED = 201,
        NO_CONTENT = 204,
        MOVED_PERMANENTLY = 301,
        FOUND = 302,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        PAYLOAD_TOO_LARGE = 413,
        INTERNAL_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        VERSION_NOT_SUPPORTED = 505,
    };
};

using namespace HttpCodes;
class HttpResponse
{
    public:
        HttpResponse(StatusCode code);

        std::string reasonPhrase(StatusCode code);
        std::string buildStatusLine();
        void printResponseHeaders();
        void handleGetRequest(const HttpRequest& request, const std::string& docRoot);
        void handlePostRequest(const HttpRequest& request);

        std::string getHttpDate();
        std::string toString();

        void setStatus(StatusCode code);
        void setHeader(const std::string& name, const std::string& value);
        void setBody(const std::string& bodyData);

        StatusCode getStatus();
        std::string getHeader(const std::string &name);
        std::string getBody();

    private:
        StatusCode _status;
        std::map<std::string, std::string> _headers;
        std::string _body;
    };

#endif
