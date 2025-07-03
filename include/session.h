#ifndef SESSION_H
# define SESSION_H

# include <string>
# include <map>

class SessionManager
{
    public:
        SessionManager();
        std::string getSessionIdFromCookie(const std::string& cookieHeader);
        std::string generateSessionId();
        const std::string& getOrCreateSession(const std::string& sessionId);

        bool sessionExists(const std::string& sessionId) const;


    private:
        std::map<std::string, std::string> _sessions;
};

#endif