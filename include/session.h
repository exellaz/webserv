#ifndef SESSION_H
# define SESSION_H

# include <string>
# include <map>

class SessionManager
{
    public:
        std::string getSessionIdFromCookie(const std::string& cookieHeader);
        std::string generateSessionId();

    private:
        std::map<std::string, std::string> sessions;
};

#endif