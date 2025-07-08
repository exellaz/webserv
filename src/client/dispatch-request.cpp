#include "Client.h"
#include "http-request.h"
#include "Cgi.hpp"
#include "session.h"

void Client::dispatchRequest()
{
    response.setHeader("Connection", request.getHeader("Connection"));
    if (request.getHeader("Connection") == "close")
        _connType = CLOSE;
    if (!location.getReturnPath().empty())
    {
        int statusCode = location.getReturnPath().begin()->first;
        std::string returnPath = location.getReturnPath().begin()->second;
        if (statusCode == MOVED_PERMANENTLY || statusCode == FOUND)
        {
            response.setStatus(static_cast<HttpCodes::StatusCode>(statusCode));
            response.setHeader("Location", returnPath);
            response.setBody("");
        }
        else if (statusCode == OK)
        {
            response.setStatus(static_cast<HttpCodes::StatusCode>(statusCode));
            response.setHeader("Content-Type", "text/plain");
            response.setBody(returnPath);

        }
    }
    else if (location.getCgiPath() == true)
    {
        std::cout << GREEN "CGI found\n" RESET;
        Cgi cgi;

        std::cout << "response body: \n" << response.getBody() << '\n';
        std::string cgiOutput = cgi.executeCgi(request, response);
        std::cerr << "cgiOutput: " << cgiOutput << "\n";

        if (cgiOutput.empty())
            throw HttpException(INTERNAL_ERROR, "CGI script execution failed");
        else {
            SessionManager& session = SessionManager::getInstance();
            handleCgiRequest(cgiOutput, response);
            if (!response.getHeader("X-Session-Update").empty())
                session.setSession(getSessionId(), response.getHeader("X-Session-Update"));
            if (response.getHeader("X-Session-Delete") == "yes")
                session.clearSessionById(getSessionId());
            std::cout << "---------- CGI Output ----------\n" << BLUE << response.toString() << RESET << "\n";
        }
    }
    else {
        if (request.getMethod() == "GET")
        {
            std::cout << GREEN "GET request\n" RESET;
            response.handleGetRequest(location, *this);
        }
        else if (request.getMethod() == "POST")
        {
            std::cout << GREEN "POST request\n" RESET;
            response.handlePostRequest(request, *this);
        }
        else if (request.getMethod() == "DELETE")
            throw HttpException(METHOD_NOT_ALLOWED, "Delete without CGI not allowed");
    }
}
