#include "http-request.h"
#include "http-response.h"
// Sample HTTP Response to POST request
// HTTP/1.1 201 Created
// Content-Type: application/json
// Location: http://example.com/users/123

// {
//   "message": "New user created",
//   "user": {
//     "id": 123,
//     "firstName": "Example",
//     "lastName": "Person",
//     "email": "bsmth@example.com"
//   }
// }

// Sample HTTP Response to GET request
// HTTP/1.1 200 OK\r\n
// Content-Type: text/plain\r\n
// Content-Length: 13\r\n
// \r\n
// Hello, world!

int main()
{
    // const std::string str("GET /index.html HTTP/1.1\r\nHost: example.com\r\nContent-Type : application/x-www-form-urlencoded\r\nContent-Length: 27\r\n\r\nfield1=value1&field2=value2\r\n");
    const std::string str("GET /index.html HTTP/1.1\r\nHost: example.com\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n");

    HttpRequest request;
    HttpResponse response(OK);

    if (!request.parseRequestLine(str)) {
        response.setStatus(NOT_IMPLEMENTED);
        std::cout << "Invalid header\n";
    }

    if (!request.parseHeaderLines(str, response)) {
        std::cout << "Invalid header field\n";
    }

    if (!request.parseRequestBody(str))
        std::cout << "No body found\n";

    std::cout << request << "\n";

    if (request.getMethod() == "GET") {
        response.handleGetRequest(request, ".");
    }
    if (request.getMethod() == "POST") {
        // response.handlePostRequest(request);
    }

    std::cout << response.toString();
    return 0;
}
