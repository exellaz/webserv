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
	else if (n == -1) {
		std::cout << "RECV_AGAIN: No data available yet, will try again next iteration.\n";
		return RECV_AGAIN;
    }

	buf[n] = '\0';
	connection.appendToBuffer(buf);
	std::cout << "curBuffer: " << connection.getBuffer() << '\n';

	return n;
}

// if actual body size is larger than contentLength, remainder is stored in buffer
int readByContentLength(Connection &conn, std::string& bodyStr)
{
	size_t bytesRead = conn.getBuffer().size();
	int ret;
	
	size_t contentLength = 100; // HARDCODED
	while (bytesRead < contentLength) {
		ret = readFromSocket(conn);
		if (ret <= 0)
			return ret; // RECV_AGAIN or RECV_CLOSED or RECV_ERROR
		conn.startTime = getNowInSeconds(); // reset timer
		bytesRead += ret;
	}
	
	if (bytesRead == contentLength) {
		bodyStr = conn.getBuffer();
		conn.clearBuffer();
	}
	else { // bytesRead > contentLength
		bodyStr = conn.getBuffer().substr(0, contentLength);
		conn.setBuffer(conn.getBuffer().substr(contentLength));
	}
	return ret;
}
int readRequestBody(Connection &conn, std::string& bodyStr)
{
	std::cout << GREY << "===== readRequestBody =====" << RESET << '\n';
	int ret;
	// if (type == CONTENT_LENGTH)
		ret = readByContentLength(conn, bodyStr);
	// else if (type == TRANSFER_ENCODING) {
	//
	// }
	// else // NO_BODY
	// 	return;
	//
	std::cout << "bodyStr size: " << bodyStr.size() << '\n';	
	std::cout << "\n===== body String: =====\n";
	std::cout << bodyStr << '\n';
	std::cout << "==========================\n\n";
	std::cout << "----Leftover in Buffer: ----\n";
	std::cout << conn.getBuffer() << '\n';
	std::cout << "----------------------------\n";

	return ret;
}


