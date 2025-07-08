#include "../../include/Cgi.hpp"
#include "session.h"

static std::map<std::string, std::string> initEnv(HttpRequest &request);
static std::string readFromFd(int fd);
static char **setEnvStrToEnvp(std::map<std::string, std::string> &env_vars,
                                        std::vector<std::string> &env_str);


Cgi::Cgi() : status(0), pid(-1), argv(NULL), envp(NULL) {
    pipefd[0] = -1;
    pipefd[1] = -1;
    script_path.clear();
    env_vars.clear();
}

std::string Cgi::executeCgi(HttpRequest &request, HttpResponse &response) {
    this->env_vars = initEnv(request);
    this->script_path = this->env_vars["SCRIPT_FILENAME"];
    if (this->script_path.find(".cgi") == std::string::npos &&
        this->script_path.find(".py") == std::string::npos &&
        this->script_path.find(".js") == std::string::npos)
    {
        response.setStatus(FORBIDDEN);
        response.setHeader("Content-Type", "text/plain");
        response.setBody("403 Forbiden: Invalid CGI script type");
        return response.toString();
    }
    std::ifstream scriptFile(this->script_path.c_str());
    if (!scriptFile.is_open()) {
        response.setStatus(FORBIDDEN);
        response.setHeader("Content-Type", "text/plain");
        response.setBody("403 Forbiden: CGI script not found");
        return response.toString();
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
        this->envp = setEnvStrToEnvp(this->env_vars, this->env_str);
        this->argv = new char*[2];
        this->argv[0] = const_cast<char*>(this->script_path.c_str());
        this->argv[1] = NULL;
        if(execve(this->script_path.c_str(), this->argv, this->envp) == -1)
            (perror("execve"), delete[] this->envp, delete[] this->argv, exit(1));
    } else {
        if (request.getMethod() == "POST" || request.getMethod() == "DELETE")
            write(this->pipefd[1], request.getBody().c_str(), request.getBody().size());
        close(this->pipefd[1]);
        waitpid(this->pid, &status, 0);
        std::string output = readFromFd(this->pipefd[0]);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0 || output.empty())
            return "";
        return (output);
    }
    return "";
}

////////////////////////////////////////////// HELPER FUNCTIONS /////////////////////////////////////////////////////////////

static std::map<std::string, std::string> initEnv(HttpRequest &request) {
    std::map<std::string, std::string> env_vars;
    SessionManager& session = SessionManager::getInstance();

    env_vars["REQUEST_METHOD"] = request.getMethod();
    env_vars["QUERY_STRING"] = request.getQueryString();
    env_vars["CONTENT_TYPE"] = request.getHeader("Content-Type");
    env_vars["CONTENT_LENGTH"] = request.getHeader("Content-Length");
    env_vars["REQUEST_URI"] = request.getURI();
    env_vars["SCRIPT_NAME"] = request.getURI();
    env_vars["SCRIPT_FILENAME"] = getFullPath(request.getURI());
    env_vars["HTTP_VERSION"] = request.getVersion();
    env_vars["SESSION_DATA"] = session.getSessionData(request.getSessionId());

    return env_vars;
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
 * @param env_vars map
 * @return char**
*/
static char **setEnvStrToEnvp(std::map<std::string, std::string> &env_vars, std::vector<std::string> &env_str) {
    char **envp = NULL;
    for (std::map<std::string, std::string>::const_iterator it = env_vars.begin(); it != env_vars.end(); ++it)
        env_str.push_back(it->first + "=" + it->second);
    envp = new char*[env_str.size() + 1];
    for (size_t i = 0; i < env_str.size(); ++i)
        envp[i] = const_cast<char*>(env_str[i].c_str());
    envp[env_str.size()] = NULL;
    return envp;
}

/**
 * @brief get full path of the file
 * @param file name of the file
 * @return full path of the file
*/
std::string getFullPath(const std::string &file) {
    std::string full_path;
    char *cwd = getcwd(NULL, 0);
    if (!cwd) {
        perror("getcwd");
        return "";
    }
    full_path = std::string(cwd) + file;
    free(cwd);
    return full_path;
}