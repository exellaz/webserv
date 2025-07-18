#ifndef HTTP_RESPONSE_H
# define HTTP_RESPONSE_H

# include <iostream>
# include <sstream>
# include <ctime>
# include <map>

# include "color.h"
# include "utils.h"

class Server;
class Client;
class Location;
class HttpRequest;
class HttpException;

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
        GATEWAY_TIMEOUT = 504,
        VERSION_NOT_SUPPORTED = 505
    };
};

using namespace HttpCodes;
class HttpResponse
{
    public:
        HttpResponse();
        HttpResponse(StatusCode code);
        HttpResponse(const HttpResponse& other);
        HttpResponse& operator=(const HttpResponse& other);
        ~HttpResponse();

        std::string reasonPhrase(StatusCode code) const;
        std::string buildStatusLine() const;
        void printResponseHeaders();
        void appendToBody(const std::string& bodyData);
        void handleGetRequest(const Location& location, const Client& client);
        void handlePostRequest(const HttpRequest& request, const Client& client);
        void clearResponse();

        std::string getHttpDate();
        std::string toString();

        void setStatus(StatusCode code);
        void setHeader(const std::string& name, const std::string& value);
        void setBody(const std::string& bodyData);

        StatusCode getStatus();
        std::string getHeader(const std::string &name);
        const std::map<std::string, std::string>& getHeaders() const;
        std::string getBody();

    private:
        StatusCode _status;
        std::map<std::string, std::string> _headers;
        std::string _body;
};

int handleParsingError(const HttpException& e, HttpResponse& response, Client& client);

std::ostream& operator<<(std::ostream &stream, const HttpResponse& src);

#endif
