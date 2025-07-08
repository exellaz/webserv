#include "../../include/Cgi.hpp"

static std::string parseBody(std::istringstream &cgi_output, std::string &line);
static std::map<std::string, std::string> parseHeader(std::istringstream &cgi_output, std::string &line);

/**
 * @brief Convert CGI output to HTTP response format
 * @param output output from the CGI script
 * @param env_vars environment variables
 * @return http response string
*/
void handleCgiRequest(const std::string &output, HttpResponse &response) {
    std::istringstream cgi_output(output);
    std::string line;
    std::map<std::string, std::string> headers;
    std::string body;

    headers = parseHeader(cgi_output, line);
    body = parseBody(cgi_output, line);

    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        if (it->first == "Status" && !it->second.empty()) {
            int code = std::atoi(it->second.c_str());
            response.setStatus(static_cast<StatusCode>(code));
        } else if (response.getStatus() != OK) {
            response.setStatus(response.getStatus());
        } else
            response.setStatus(OK);

        response.setHeader(it->first, it->second);
    }

    response.setBody(body);
}

/////////////////////////////////////// HELPER FUNCTION ///////////////////////////////////////////////////

/**
 * @brief Parse CGI output to extract headers
 * @param cgi_output stream of CGI output
 * @param line reference the line to read from the stream
 * @param status_line status line
 * @note value loop is to remove the whitespace at the beginning of the value
 * @return header
*/
static std::map<std::string, std::string> parseHeader(std::istringstream &cgi_output, std::string &line) {
    std::map<std::string, std::string> headers;
    while (std::getline(cgi_output, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (line.empty())
            break;
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            while (!value.empty() && value[0] == ' ')
                value.erase(0, 1);
            headers[key] = value;
        }
    }
    return headers;
}

/**
 * @brief Parse CGI output to extract body
 * @param cgi_output stream of CGI output
 * @param line reference the line to read from the stream
 * @return body
*/
static std::string parseBody(std::istringstream &cgi_output, std::string &line) {
    std::string body;
    while (std::getline(cgi_output, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        body += line + "\n";
    }
    return body;
}