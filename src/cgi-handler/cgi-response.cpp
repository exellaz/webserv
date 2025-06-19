#include "../../include/Cgi.hpp"

static std::string parseBody(std::istringstream &cgi_output, std::string &line);
static std::map<std::string, std::string> parseHeader(std::istringstream &cgi_output,
														std::string &line,
														std::string &status_line);

/**
 * @brief Convert CGI output to HTTP response format
 * @param output output from the CGI script
 * @param env_vars environment variables
 * @return http response string
*/
void handleCgiRequest(const std::string &output, HttpResponse &response)
{
	std::istringstream cgi_output(output);
	std::string line;
	std::map<std::string, std::string> headers;
	std::string body;
	std::string status_line;

	headers = parseHeader(cgi_output, line, status_line);
	body = parseBody(cgi_output, line);


	//set status
	if (!status_line.empty())
	{
		int code = std::atoi(status_line.c_str());
		response.setStatus(static_cast<StatusCode>(code));
	}
	else
		response.setStatus(OK);

	//set headers
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
		 it != headers.end(); ++it)
		response.setHeader(it->first, it->second);

	//set body
	response.setBody(body);
}

/////////////////////////////////////// HELPER FUNCTION ///////////////////////////////////////////////////

/**
 * @brief Parse CGI output to extract headers
 * @param cgi_output stream of CGI output
 * @param line reference the line to read from the stream
 * @param status_line status line
 * @return header
*/
static std::map<std::string, std::string> parseHeader(std::istringstream &cgi_output,
														std::string &line,
														std::string &status_line)
{
	std::map<std::string, std::string> headers;
	for (; std::getline(cgi_output, line);)
	{
		if (!line.empty() && line[line.size() - 1] == '\r') //remove trailing \r
			line.erase(line.size() - 1);
		if (line.empty()) //check for end of header
			break;
		size_t colon = line.find(':');
		if (colon != std::string::npos)
		{
			std::string key = line.substr(0, colon);
			std::string value = line.substr(colon + 1);
			while (!value.empty() && value[0] == ' ') //trim the leading space
				value.erase(0, 1);
			if (key == "Status") //check for status
				status_line = value;
			else
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
static std::string parseBody(std::istringstream &cgi_output, std::string &line)
{
	std::string body;
	for (;std::getline(cgi_output, line);)
	{
		if (!line.empty() && line[line.size() - 1] == '\r') //remove trailing \r
			line.erase(line.size() - 1);
		body += line + "\n";
	}
	return body;
}