#include "session.h"

std::string generateSessionId()
{
    static const std::string& alphaNum =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string sessionId;
    for (int i = 0; i < 32; ++i)
        sessionId += alphaNum[std::rand() % alphaNum.size()];
    return sessionId;
}

std::string getSessionIdFromCookie(const std::string& cookieHeader)
{
    std::string key = "session_id=";
    std::size_t start = cookieHeader.find(key);
    if (start == std::string::npos)
        return "";
    start += key.length();
    std::size_t end = cookieHeader.find(';', start);
    return cookieHeader.substr(start, end - start);
}
