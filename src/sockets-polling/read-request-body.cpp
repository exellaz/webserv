#include "../../include/sockets-polling.h"

static int readFromSocket(Connection &connection)
{
	char buf[BODY_BUFFER_SIZE + 1];

	ssize_t n = recv(connection.fd, buf, BODY_BUFFER_SIZE, 0);
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
	std::cout << "curBuffer: " << connection.getBuffer() << '\n';

	return n;
}

int readByContentLength(Connection &conn, std::string& bodyStr)
{
	size_t bytesRead = conn.getBuffer().size();
	int ret;
	
	size_t contentLength = 10; // HARDCODED
	while (bytesRead < contentLength) {
		ret = readFromSocket(conn);
		if (ret <= 0)
			return ret; // RECV_AGAIN or RECV_CLOSED or RECV_ERROR
		bytesRead += ret;
	}

	bodyStr = conn.getBuffer();
	conn.clearBuffer();
	return ret;
}
int readRequestBody(Connection &conn, std::string& bodyStr)
{
	std::cout << GREY << "===== readRequestBody =====" << RESET << '\n';

	// if (type == CONTENT_LENGTH)
		return readByContentLength(conn, bodyStr);
	// else if (type == TRANSFER_ENCODING) {
	//
	// }
	// else // NO_BODY
	// 	return;
	//
	std::cout << "\n===== body String: =====\n";
	std::cout << bodyStr << '\n';
	std::cout << "==========================\n\n";
	
	return 0;
}


