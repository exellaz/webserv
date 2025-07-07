#include "../../include/Cgi.hpp"
#include "session.h"

static std::map<std::string, std::string> initEnv(HttpRequest &request);
static std::string readFromFd(int fd);
static char **setEnvStrToEnvp(std::map<std::string, std::string> &env_vars,
										std::vector<std::string> &env_str);


Cgi::Cgi() : status(0), pid(-1), argv(NULL), envp(NULL)
{
	pipefd[0] = -1;
	pipefd[1] = -1;
	script_path.clear();
	env_vars.clear();
}

std::string Cgi::executeCgi(HttpRequest &request, HttpResponse &response)
{
	Cgi cgi;

	cgi.env_vars = initEnv(request);
	cgi.script_path = cgi.env_vars["SCRIPT_FILENAME"];
	if (cgi.script_path.find(".cgi") == std::string::npos &&
		cgi.script_path.find(".py") == std::string::npos &&
		cgi.script_path.find(".js") == std::string::npos)
	{
		//return "Status: 403 Forbiden\r\nContent-Type: text/plain\r\n\r\n403 Forbiden: Invalid CGI script type"; //! error
		response.setStatus(FORBIDDEN);
		response.setHeader("Content-Type", "text/plain");
		response.setBody("403 Forbiden: Invalid CGI script type");
		return response.toString(); //! error
	}
	std::ifstream scriptFile(cgi.script_path.c_str());
	if (!scriptFile.is_open())
	{
		//return "Status: 403 Forbiden\r\nContent-Type: text/plain\r\n\r\n403 Forbiden: CGI script not found"; //! error
		response.setStatus(FORBIDDEN);
		response.setHeader("Content-Type", "text/plain");
		response.setBody("403 Forbiden: CGI script not found");
		return response.toString(); //! error
	}
	if (pipe(cgi.pipefd) == -1)
		throw std::runtime_error("pipe error");
	cgi.pid = fork();
	if (cgi.pid == -1)
		throw std::runtime_error("fork error");
	if (cgi.pid == 0)
	{
		dup2(cgi.pipefd[0], STDIN_FILENO); // Redirect stdin to pipe
		dup2(cgi.pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
		close(cgi.pipefd[0]); // Close read end
		close(cgi.pipefd[1]); // Close write end
		cgi.envp = setEnvStrToEnvp(cgi.env_vars, cgi.env_str);
		cgi.argv = new char*[2];
		cgi.argv[0] = const_cast<char*>(cgi.script_path.c_str());
		cgi.argv[1] = NULL;
		if(execve(cgi.script_path.c_str(), cgi.argv, cgi.envp) == -1)
			(perror("execve"), delete[] cgi.envp, delete[] cgi.argv, exit(1));
	}
	else
	{
		if (request.getMethod() == "POST" || request.getMethod() == "DELETE")
			write(cgi.pipefd[1], request.getBody().c_str(), request.getBody().size());
		close(cgi.pipefd[1]);
		waitpid(cgi.pid, &status, 0);
		std::string output = readFromFd(cgi.pipefd[0]);
		//if exit child is not exited normally || exit code is not 0 || output is empty
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0 || output.empty())
			return "";
		return (output);
	}
	return "";
}

////////////////////////////////////////////// HELPER FUNCTIONS /////////////////////////////////////////////////////////////

static std::map<std::string, std::string> initEnv(HttpRequest &request)
{
	std::map<std::string, std::string> env_vars;
	SessionManager& session = SessionManager::getInstance();

	//assign method, url, version, headers and body to env
	env_vars["REQUEST_METHOD"] = request.getMethod(); //request method (GET, POST, DELETE, etc.)
	env_vars["QUERY_STRING"] = request.getQueryString();
	env_vars["CONTENT_TYPE"] = request.getHeader("Content-Type"); // script body type
	env_vars["CONTENT_LENGTH"] = request.getHeader("Content-Length"); // script body length
	env_vars["REQUEST_URI"] = request.getURI(); //request url (location url)
	env_vars["SCRIPT_NAME"] = request.getURI(); //script name (location url)
	env_vars["SCRIPT_FILENAME"] = getFullPath(request.getURI()); //full path of the script (sript full path)
	env_vars["HTTP_VERSION"] = request.getVersion(); //HTTP version (HTTP/1.1, HTTP/2, etc.)
	env_vars["SESSION_DATA"] = session.getSessionData(request.getSessionId());

	return env_vars;
}

/**
 * @brief read from file descriptor
 * @param fd file descriptor
 * @return string read from file descriptorx
*/
static std::string readFromFd(int fd)
{
	char buf[4096];
	int len = 0;
	std::string output;
	while (true)
	{
		len = read(fd, buf, sizeof(buf) - 1);
		if (len <= 0)
			break;
		output.append(buf, len); // Append to output string
	}
	close(fd); // Close the file descriptor
	return output;
}

/**
 * @brief convert map to char **
 * @param env_vars map
 * @return char**
*/
static char **setEnvStrToEnvp(std::map<std::string, std::string> &env_vars, std::vector<std::string> &env_str)
{
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
std::string getFullPath(const std::string &file)
{
	std::string full_path;
	char *cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("getcwd");
		return "";
	}
	full_path = std::string(cwd) + file;
	free(cwd);
	return full_path;
}