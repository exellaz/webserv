#include "session.h"
#include <cstdlib>
#include <ctime>

SessionManager::SessionManager()
{
    std::srand(std::time(0));
}

std::string SessionManager::generateSessionId()
{
    static const std::string& alphaNum =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string sessionId;
    for (int i = 0; i < 32; ++i)
        sessionId += alphaNum[std::rand() % alphaNum.size()];
    return sessionId;
}

std::string SessionManager::getSessionIdFromCookie(const std::string& cookieHeader)
{
    std::string key = "session_id=";
    std::size_t start = cookieHeader.find(key);
    if (start == std::string::npos)
        return "";
    start += key.length();
    std::size_t end = cookieHeader.find(';', start);
    return cookieHeader.substr(start, end - start);
}

const std::string& SessionManager::getOrCreateSession(const std::string& sessionId)
{
    if (sessionId.empty() || !sessionExists(sessionId)) {
        std::string newId = generateSessionId();
        _sessions[newId] = "guest"; // default
        return _sessions[newId];
    }
    return _sessions[sessionId];
}

std::string SessionManager::buildSetCookieHeader(const std::string& sessionId) const
{
    return "Set-Cookie: session_id=" + sessionId + "; Path=/; HttpOnly";
}

bool SessionManager::sessionExists(const std::string& sessionId) const
{
    return _sessions.find(sessionId) != _sessions.end();
}
