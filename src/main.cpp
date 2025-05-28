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
    const std::string str("GET /index.html HTTP/1.1\r\nHost: www.example.re\r\nUser-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.0; en-US; rv:1.1)\r\nAccept: text/html\r\nAccept-Language: en-US, en; q=0.5\r\nAccept-Encoding: gzip, deflate\r\n");
    HttpRequest request;

    if (!request.parseRequestLine(str)) {
        std::cout << "Invalid header" << "\n";
        return false;
    }
    std::cout << "Method: " << request._method << "\n";
    std::cout << "URL: " << request._url << "\n";
    std::cout << "Version: " << request._version << "\n";
    return 0;
}