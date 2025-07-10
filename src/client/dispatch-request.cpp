#include "Client.h"
#include "http-request.h"
#include "Cgi.hpp"
#include "session.h"

void Client::dispatchRequest()
{
    if (request.getHeader("Connection") == "close") {
        response.setHeader("Connection", "close");
        _connType = CLOSE;
    }
    else
        response.setHeader("Connection", "keep-alive");
    
    if (!location.getReturnPath().empty()) {
        int statusCode = location.getReturnPath().begin()->first;
        std::string returnPath = location.getReturnPath().begin()->second;
        if (statusCode == MOVED_PERMANENTLY || statusCode == FOUND) {
            response.setStatus(static_cast<HttpCodes::StatusCode>(statusCode));
            response.setHeader("Location", returnPath);
            response.setBody("");
        }
        else if (statusCode == OK) {
            response.setStatus(static_cast<HttpCodes::StatusCode>(statusCode));
            response.setHeader("Content-Type", "text/plain");
            response.setBody(returnPath);
        }
    }
    else if (location.getCgiPath() == true) {
        Cgi cgi;
        std::string cgiOutput = cgi.executeCgi(request);

        if (cgiOutput.empty())
            throw HttpException(INTERNAL_ERROR, "CGI script execution failed");
        else {
            SessionManager& session = SessionManager::getInstance();
            handleCgiRequest(cgiOutput, response);
            if (!response.getHeader("X-Session-Update").empty())
                session.setSession(getSessionId(), response.getHeader("X-Session-Update"));
            if (response.getHeader("X-Session-Delete") == "yes")
                session.clearSessionById(getSessionId());
            if (response.getStatus() == OK)
                std::cout << "CGI EXECUTE: [ " << GREEN "SUCCESS" RESET << " ]\n\n";
            else
                std::cout << "CGI EXECUTE: [ " << RED "FAIL" RESET << " ]\n\n";
        }
    }
    else {
        if (request.getMethod() == "GET")
            response.handleGetRequest(location, *this);
        else if (request.getMethod() == "POST")
            response.handlePostRequest(request, *this);
        else if (request.getMethod() == "DELETE")
            throw HttpException(METHOD_NOT_ALLOWED, "Delete without CGI not allowed");
    }
}
