#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <fcntl.h>

std::string executeCgi(const std::string &scriptPath, const std::string &queryString) {
    int pipefd[2];
    if (pipe(pipefd) == -1)
        return "Status: 500 Internal Server Error\r\n\r\nPipe failed.";

    pid_t pid = fork();

    if (pid == -1) {
        return "Status: 500 Internal Server Error\r\n\r\nFork failed.";
    }

    if (pid == 0) {
        // Child
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(pipefd[0]); // Close read end

        // Set environment
        setenv("REQUEST_METHOD", "GET", 1);
        setenv("QUERY_STRING", queryString.c_str(), 1);
        setenv("SCRIPT_FILENAME", scriptPath.c_str(), 1);

        char *argv[] = { (char *)scriptPath.c_str(), NULL };
        execve(scriptPath.c_str(), argv, environ); // Run script

        // If exec fails
        exit(1);
    } else {
        // Parent
        close(pipefd[1]); // Close write end
        waitpid(pid, NULL, 0); // Wait for child

        // Read output
        char buffer[1024];
        std::string result;
        ssize_t bytes;
        while ((bytes = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            result.append(buffer, bytes);
        }
        close(pipefd[0]);

        return result;
    }
}

int main()
{
	std::string response = executeCgi("./cgi-bin/hello.py", "name=john");
	std::cout << "CGI Response:\n" << response << std::endl;
}
