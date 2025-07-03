#include "session.h"
#include <cstdlib>
#include <ctime>

SessionManager::SessionManager()
{
    std::srand(std::time(0));
}

void SessionManager::handleSession(Client& client)
{
    static SessionManager sessionMgr;
    HttpRequest& request = client.request;
    HttpResponse& response = client.response;

    std::string cookieHeader = request.getHeader("Cookie");
    std::string sessionId = sessionMgr.getSessionIdFromCookie(cookieHeader);
    if (!sessionMgr.sessionExists(sessionId)) {
        sessionId = generateSessionId();
        response.setHeader("Set-Cookie", buildSetCookieHeader(sessionId));
        sessionMgr._sessions[sessionId] = "default";
    }
    client.setSessionId(sessionId);
    client.setSessionData(sessionMgr.getSessionData(sessionId));
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

// sessionData can be a struct instead to hold all necessary data of the session
void SessionManager::setSession(const std::string& sessionId, const std::string& sessionData)
{
    _sessions[sessionId] = sessionData;
}

const std::string& SessionManager::getSessionData(const std::string& sessionId)
{
    return _sessions[sessionId];
}

std::string SessionManager::buildSetCookieHeader(const std::string& sessionId)
{
    return "session_id=" + sessionId + "; Path=/; HttpOnly";
}

bool SessionManager::sessionExists(const std::string& sessionId) const
{
    return _sessions.find(sessionId) != _sessions.end();
}
