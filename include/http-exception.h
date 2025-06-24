#ifndef HTTP_EXCEPTION_H
# define HTTP_EXCEPTION_H

# include <stdexcept>
# include "http-response.h"

class HttpException : public std::exception
{
    public:
        HttpException(StatusCode statusCode, const std::string& message)
            : _code(statusCode), _message(message) {}

        virtual ~HttpException() throw() {};

        const char* what() const throw() {
            return _message.c_str();
        }

        StatusCode getStatusCode() const { return _code; }

    private:
        StatusCode _code;
        std::string _message;
};

#endif