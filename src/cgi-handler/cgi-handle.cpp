#include "../../include/Cgi.hpp"
#include "session.h"

static std::map<std::string, std::string> initEnv(HttpRequest &request);
static std::string readFromFd(int fd);
static char **setEnvStrToEnvp(std::map<std::string, std::string> &envVars,
                                        std::vector<std::string> &envStr);
static int timerWaitpid(pid_t pid, int *status, int timeout);

Cgi::Cgi() : status(0), pid(-1), argv(NULL), envp(NULL) {
    pipefd[0] = -1;
    pipefd[1] = -1;
    scriptPath.clear();
    envVars.clear();
}

std::string Cgi::executeCgi(HttpRequest &request) {
    this->envVars = initEnv(request);
    this->scriptPath = this->envVars["SCRIPT_FILENAME"];
    if (this->scriptPath.find(".cgi") == std::string::npos &&
        this->scriptPath.find(".py") == std::string::npos &&
        this->scriptPath.find(".js") == std::string::npos)
    {
        throw HttpException(FORBIDDEN, "CGI script must have .cgi, .py or .js extension");
    }
    std::ifstream scriptFile(this->scriptPath.c_str());
    if (!scriptFile.is_open()) {
        throw HttpException(FORBIDDEN, "CGI script not found");
    }
    if (pipe(this->pipefd) == -1)
        throw std::runtime_error("pipe error");
    this->pid = fork();
    if (this->pid == -1)
        throw std::runtime_error("fork error");
    if (this->pid == 0) {
        dup2(this->pipefd[0], STDIN_FILENO);
        dup2(this->pipefd[1], STDOUT_FILENO);
        close(this->pipefd[0]);
        close(this->pipefd[1]);
        std::string scriptDir = this->scriptPath.substr(0, this->scriptPath.find_last_of('/'));
        if (chdir(scriptDir.c_str()) == -1) {
            perror("chdir failed");
            exit(1);
        }
        this->envp = setEnvStrToEnvp(this->envVars, this->envStr);
        this->argv = new char*[2];
        this->argv[0] = const_cast<char*>(this->scriptPath.c_str());
        this->argv[1] = NULL;
        if(execve(this->scriptPath.c_str(), this->argv, this->envp) == -1)
            (perror("execve"), delete[] this->envp, delete[] this->argv, exit(1));
    } else {
        if (request.getMethod() == "POST" || request.getMethod() == "DELETE")
            write(this->pipefd[1], request.getBody().c_str(), request.getBody().size());
        close(this->pipefd[1]);
        int timeout = timerWaitpid(this->pid, &this->status, 5);
        if (timeout == -2)
            throw HttpException(GATEWAY_TIMEOUT, "CGI script execution timed out");
        std::string output = readFromFd(this->pipefd[0]);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0 || output.empty())
            return "";
        return (output);
    }
    return "";
}

////////////////////////////////////////////// HELPER FUNCTIONS /////////////////////////////////////////////////////////////

static std::map<std::string, std::string> initEnv(HttpRequest &request) {
    std::map<std::string, std::string> envVars;
    SessionManager& session = SessionManager::getInstance();

    envVars["REQUEST_METHOD"] = request.getMethod();
    envVars["QUERY_STRING"] = request.getQueryString();
    envVars["CONTENT_TYPE"] = request.getHeader("Content-Type");
    envVars["CONTENT_LENGTH"] = request.getHeader("Content-Length");
    envVars["REQUEST_URI"] = request.getURI();
    envVars["SCRIPT_NAME"] = request.getURI();
    envVars["SCRIPT_FILENAME"] = getFullPath(request.getURI());
    envVars["HTTP_VERSION"] = request.getVersion();
    envVars["SESSION_DATA"] = session.getSessionData(request.getSessionId());

    return envVars;
}

/**
 * @brief read from file descriptor
 * @param fd file descriptor
 * @return string read from file descriptorx
*/
static std::string readFromFd(int fd) {
    char buf[4096];
    int len = 0;
    std::string output;
    while (true) {
        len = read(fd, buf, sizeof(buf) - 1);
        if (len <= 0)
            break;
        output.append(buf, len);
    }
    close(fd);
    return output;
}

/**
 * @brief convert map to char **
 * @param envVars map
 * @return char**
*/
static char **setEnvStrToEnvp(std::map<std::string, std::string> &envVars, std::vector<std::string> &envStr) {
    char **envp = NULL;
    for (std::map<std::string, std::string>::const_iterator it = envVars.begin(); it != envVars.end(); ++it)
        envStr.push_back(it->first + "=" + it->second);
    envp = new char*[envStr.size() + 1];
    for (size_t i = 0; i < envStr.size(); ++i)
        envp[i] = const_cast<char*>(envStr[i].c_str());
    envp[envStr.size()] = NULL;
    return envp;
}

static int timerWaitpid(pid_t pid, int *status, int timeout)
{
    int elapsed = 0;
    while (elapsed < timeout) {
        pid_t ret = waitpid(pid, status, WNOHANG);
        if (ret == pid)
            return ret;
        else if (ret == 0)
            sleep(1);
        else
            return -1;
        ++elapsed;
    }
    kill(pid, SIGKILL);
    waitpid(pid, status, 0);
    return -2;
}
