#include "http-request.h"
#include "Cgi.hpp"
#include "read-request.h"

void dispatchRequest(Client& client)
{
    HttpRequest& request = client.request;
    HttpResponse& response = client.response;

    response.setHeader("Connection", request.getHeader("Connection"));
    if (request.getHeader("Connection") == "close")
        client.connType = CLOSE;
    if (!client.location.getReturnPath().empty())
    {
        int statusCode = client.location.getReturnPath().begin()->first;
        std::string returnPath = client.location.getReturnPath().begin()->second;
        if (statusCode == MOVED_PERMANENTLY || statusCode == FOUND)
        {
            client.response.setStatus(static_cast<HttpCodes::StatusCode>(statusCode));
            client.response.setHeader("Location", returnPath);
            client.response.setBody("");
        }
        else if (statusCode == OK)
        {
            client.response.setStatus(static_cast<HttpCodes::StatusCode>(statusCode));
            client.response.setHeader("Content-Type", "text/plain");
            client.response.setBody(returnPath);

        }
    }
    else if (client.location.getCgiPath() == true)
    {
        std::cout << GREEN "CGI found\n" RESET;
        Cgi cgi;

        std::cout << "response body: \n" << response.getBody() << '\n';
        std::string cgiOutput = cgi.executeCgi(request, response);
        std::cerr << "cgiOutput: " << cgiOutput << "\n";

        if (cgiOutput.empty())
            throw HttpException(INTERNAL_ERROR, "CGI script execution failed");
        else {
            handleCgiRequest(cgiOutput, response);
            std::cout << "---------- CGI Output ----------\n" << BLUE << response.toString() << RESET << "\n";
        }
    }
    else {
        if (request.getMethod() == "GET")
        {
            std::cout << GREEN "GET request\n" RESET;
            response.handleGetRequest(client.location, client);
        }
        else if (request.getMethod() == "POST")
        {
            std::cout << GREEN "POST request\n" RESET;
            response.handlePostRequest(request, client);
        }
        else if (request.getMethod() == "DELETE")
            throw HttpException(METHOD_NOT_ALLOWED, "Delete without CGI not allowed");
    }
}
