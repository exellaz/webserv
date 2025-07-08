#include "http-request.h"
#include "Cgi.hpp"
#include "read-request.h"
#include "session.h"

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
        std::cout << GREY "\n===== CGI INFO =====\n" RESET;
        Cgi cgi;

        std::string cgiOutput = cgi.executeCgi(request, response);

        if (cgiOutput.empty())
            throw HttpException(INTERNAL_ERROR, "CGI script execution failed");
        else {
            SessionManager& session = SessionManager::getInstance();
            handleCgiRequest(cgiOutput, response);
            if (!response.getHeader("X-Session-Update").empty())
                session.setSession(client.getSessionId(), response.getHeader("X-Session-Update"));
            if (response.getHeader("X-Session-Delete") == "yes")
                session.clearSessionById(client.getSessionId());
            if (response.getStatus() == OK)
                std::cout << "CGI EXECUTE: [ " << GREEN "SUCCESS" RESET << " ]\n\n";
            else
                std::cout << "CGI EXECUTE: [ " << RED "FAIL" RESET << " ]\n\n";
            //std::cout << "---------- CGI Output ----------\n" << BLUE << response.toString() << RESET << "\n";
        }
    }
    else {
        if (request.getMethod() == "GET")
            response.handleGetRequest(client.location, client);
        else if (request.getMethod() == "POST")
            response.handlePostRequest(request, client);
        else if (request.getMethod() == "DELETE")
            throw HttpException(METHOD_NOT_ALLOWED, "Delete without CGI not allowed");
    }
}
