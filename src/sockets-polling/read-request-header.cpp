#include "../../include/sockets-polling.h"


static int readFromSocket(Connection &connection)
{
	char buf[HEADER_BUFFER_SIZE + 1];

	ssize_t n = recv(connection.fd, buf, HEADER_BUFFER_SIZE, 0);
	std::cout << "n: " << n << '\n';
	
    if (n == 0) {
		std::cout << "RECV_CLOSED\n";
		return RECV_CLOSED;
	}
    if (n == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {// WARN: cannot use `errno` after read
			std::cout << "RECV_AGAIN\n";
            return RECV_AGAIN;
		}
        perror("recv error");
		std::cout << "RECV_ERROR\n";
        return RECV_ERROR;
    }

	buf[n] = '\0';
	connection.appendToBuffer(buf);

	return n;
}

int readRequestHeader(Connection &conn, std::string& headerStr)
{
	std::cout << GREY << "===== readRequestHeader =====" << RESET << '\n';

	size_t found;
	int ret;

	while (1) {

		ret = readFromSocket(conn);
		if (ret > 0) {
			found = conn.getBuffer().find(HEADER_END);
			if (found != std::string::npos)
				break;
			else 
				continue;
		}
		else 
			return ret; // RECV_AGAIN or RECV_CLOSED or RECV_ERROR
	}


	const std::string& buffer = conn.getBuffer();

	// TODO: if CRLF not found -> throw exception?
	if (found == std::string::npos) {
		std::cout << "getHeaderStr: Error: End of Header not found\n";

	}
	// if no body
	if (buffer.begin() + found == buffer.end() - 4) {
		std::cout << "getHeaderStr: no body found\n";
		headerStr = buffer.substr(0, buffer.length() - 4); // exclude "abc\r\n\r\n abc"
		conn.clearBuffer();
	}
	else {
		headerStr = buffer.substr(0, found);
		// replace buffer with body part only
		conn.setBuffer(buffer.substr(found + 4));
	}

	std::cout << "\n===== Header String: =====\n";
	std::cout << headerStr << '\n';
	std::cout << "==========================\n\n";
	std::cout << "----Leftover in Buffer: ----\n";
	std::cout << buffer << '\n';
	std::cout << "----------------------------\n";
	return ret;
}

