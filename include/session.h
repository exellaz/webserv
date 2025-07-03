#ifndef SESSION_H
# define SESSION_H

# include <string>
# include <map>
# include "Client.h"

class SessionManager
{
    public:
        SessionManager();
        static std::string getSessionIdFromCookie(const std::string& cookieHeader);
        static std::string generateSessionId();
        const std::string& getSessionData(const std::string& sessionId);
        static std::string buildSetCookieHeader(const std::string& sessionId);
        static void handleSession(Client& client);
        void setSession(const std::string& sessionId, const std::string& sessionData);

        bool sessionExists(const std::string& sessionId) const;

    private:
        std::map<std::string, std::string> _sessions;
};

#endif