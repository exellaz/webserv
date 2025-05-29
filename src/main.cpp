#include "HttpRequest.h"

// Sample HTTP Request:
// GET /index.html HTTP/1.1
// Host: www.example.re
// User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.0; en-US; rv:1.1)
// Accept: text/html
// Accept-Language: en-US, en; q=0.5
// Accept-Encoding: gzip, deflate

int main()
{
    // const std::string str("GET /index.html HTTP/1.1\r\nHost: localhost\r\nUser-Agent: Mozzila/5.0\r\n");
    // const std::string str("GET /index.html HTTP/1.1\r\nHost: www.example.re\r\nUser-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.0; en-US; rv:1.1)\r\nAccept: text/html\r\nAccept-Language: en-US, en; q=0.5\r\nAccept-Encoding: gzip, deflate\r\n");
    const std::string str("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 27\r\n\r\nfield1=value1&field2=value2\r\n");
    HttpRequest request;

    if (!request.parseRequestLine(str)) {
        std::cout << "Invalid header\n";
        return 1;
    }

    std::istringstream stream(str);
    std::string line;
    while (std::getline(stream, line) && line != "\r") {
        if (!line.empty() && line[line.length() - 1] == '\r') {
            line = line.substr(0, line.length() - 1);
        }
        request.parseHeaderLine(line);
    }

    if (!request.parseRequestBody(str))
        std::cout << "No body found\n";

    std::cout << request;
    return 0;
}
