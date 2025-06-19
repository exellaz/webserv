
#include <iostream>
#include <fstream>
#include <string>

int main() {
    // 1. Required CGI header
    std::cout << "Content-Type: text/html\r\n\r\n";

    // 2. Open your HTML file
    std::ifstream htmlFile("/home/welow/nginx/main.html");
    if (!htmlFile) {
        std::cout << "<html><body><h1>Error</h1><p>Could not load HTML file.</p></body></html>";
        return 1;
    }

    // 3. Stream file contents to stdout
    std::string line;
    while (std::getline(htmlFile, line)) {
        std::cout << line << "\n";
    }

    htmlFile.close();
    return 0;
}
