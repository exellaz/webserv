#include "../../include/sockets-polling.h"

static int readFromSocket(Connection &connection, int bufferSize)
{
	char buf[BODY_BUFFER_SIZE + 1];

	ssize_t n = recv(connection.fd, buf, bufferSize, 0);
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
	connection.appendToBuffer(buf, n);
	std::cout << "curBuffer: " << connection.getBuffer() << '\n';

	return n;
}

// if actual body size is larger than contentLength, remainder is stored in buffer
int readByContentLength(Connection &conn, std::string& bodyStr, int bufferSize)
{
	size_t bytesRead = conn.getBuffer().size();
	int ret;

	size_t contentLength = std::strtol(conn.request.getHeader("Content-Length").c_str(), NULL, 10); // HARDCODED
	while (bytesRead < contentLength) {
		ret = readFromSocket(conn, bufferSize);
		if (ret <= 0)
		return ret; // RECV_AGAIN or RECV_CLOSED or RECV_ERROR
		conn.startTime = getNowInSeconds(); // reset timer
		bytesRead += ret;
		std::cout << "bytesRead: " << bytesRead << '\n'; ////debug
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
int readRequestBody(Connection &conn, std::string& bodyStr, int bufferSize)
{
	std::cout << GREY << "===== readRequestBody =====" << RESET << '\n';
	int ret;
	if (conn.readBodyMethod == CONTENT_LENGTH) {
		std::cout << "CONTENT_LENGTH\n";
		ret = readByContentLength(conn, bodyStr, bufferSize);
	}
	else if (conn.readBodyMethod == CHUNKED_ENCODING) {
		std::cout << "CHUNKED ENCODING\n";
		ret = readByChunkedEncoding(conn, bodyStr, bufferSize);
	}
	else {// NO_BODY
		std::cout << "NO BODY\n";
		return RECV_OK;
	}

	return ret;
}


