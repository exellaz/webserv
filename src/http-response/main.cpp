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
    // const std::string str("GET /index.html HTTP/1.1\r\nHost: example.com\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n");
    const std::string str("POST /upload HTTP/1.1\r\nContent-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW\r\n\r\n");
    const std::string bodyStr = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"text_file\"; filename=\"sample.txt\"\r\nContent-Type: text/plain\r\n\r\nThis file was uploaded by the handler!\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"text_file2\"; filename=\"sample2.txt\"\r\nContent-Type: text/plain\r\n\r\nThis file was also uploaded by the handler!\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--";
    HttpRequest request;
    HttpResponse response(OK);

    // request.parseRequestLine(str);
    // request.parseHeaderLines(str);
    request.setHeader("content-type", "multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
    request.setBody(bodyStr);
    request.setMethod("POST");

    if (request.getMethod() == "POST")
        response.handlePostRequest(request, "./objs/");

    std::cout << response.toString();
    return 0;
}
