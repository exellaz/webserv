#include "http-request.h"
#include "http-exception.h"

std::string HttpRequest::generateSessionId()
{
    std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string sessionId;
    for (int i = 0; i < 32; ++i)
        sessionId += chars[rand() % chars.size()];
    return sessionId;
}

std::string HttpRequest::getSessionIdFromCookie(const std::string& cookieHeader)
{
    std::string key = "session_id=";
    std::size_t start = cookieHeader.find(key);
    if (start == std::string::npos)
        return "";
    start += key.length();
    std::size_t end = cookieHeader.find(';', start);
    return cookieHeader.substr(start, end - start);
}
