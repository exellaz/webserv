#ifndef SESSION_H
# define SESSION_H

# include <string>
# include <map>

# include "Client.h"

class SessionManager
{
    public:
        SessionManager();
        static SessionManager& getInstance();
        static void handleSession(Client& client);
        void clearSessionById(const std::string& sessionId);

        const std::string& getSessionData(const std::string& sessionId);

        void setSession(const std::string& sessionId, const std::string& sessionData);

    private:
        std::map<std::string, std::string> _sessions;
        bool _sessionExists(const std::string& sessionId) const;
};

#endif
