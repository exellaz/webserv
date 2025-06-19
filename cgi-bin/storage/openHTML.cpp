
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <fcntl.h>

#define PORT 1234


extern char **environ;

//for CGI execution
std::string executeCgi(const std::string &scriptPath, const std::string &queryString)
{
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

// Function to send HTTP response
void send_response(int client_socket, const std::string& path)
{
	std::ifstream file(path);
	if (!file) {
		std::string not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nFile not found";
		send(client_socket, not_found.c_str(), not_found.size(), 0);
		return;
	}

	std::stringstream content;
	content << file.rdbuf();
	std::string body = content.str();

	std::string header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	send(client_socket, header.c_str(), header.size(), 0);
	send(client_socket, body.c_str(), body.size(), 0);
}

int main() {
	int server_fd, client_socket;
	sockaddr_in address;
	int addrlen = sizeof(address);

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == 0) {
		perror("socket failed");
		return 1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	bind(server_fd, (sockaddr*)&address, sizeof(address));
	listen(server_fd, 10);

	std::cout << "Server running on http://localhost:" << PORT << "\n";

	while (true) {
		client_socket = accept(server_fd, (sockaddr*)&address, (socklen_t*)&addrlen);
		if (client_socket < 0) {
			perror("accept");
			continue;
		}

		char buffer[1024] = {0};
		read(client_socket, buffer, 1024);
		std::string request(buffer);

		// Parse requested path
		std::string path = ".";
		std::string query;
		size_t pos = request.find("GET ");
		if (pos != std::string::npos) {
			size_t start = pos + 4;
			size_t end = request.find(" ", start);
			std::string file = request.substr(start, end - start);

			// Split query string if present
			size_t qpos = file.find('?');
			if (qpos != std::string::npos) {
				query = file.substr(qpos + 1);
				file = file.substr(0, qpos);
			}

			path += (file == "/") ? "/index.html" : file;
		}


		// Check for CGI
		if (path.find("./cgi-bin/") == 0) {
			std::string cgi_output = executeCgi(path, query);

			// Check if CGI output starts with "Content-Type"
			if (cgi_output.find("Content-Type:") == 0) {
				cgi_output = "HTTP/1.1 200 OK\r\n" + cgi_output;
			}

			send(client_socket, cgi_output.c_str(), cgi_output.size(), 0);
		} else {
			send_response(client_socket, path);
		}
		close(client_socket);

	}

	return 0;
}
