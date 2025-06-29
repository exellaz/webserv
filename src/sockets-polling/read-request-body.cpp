#include "../../include/sockets-polling.h"

// if actual body size is larger than contentLength, remainder is stored in buffer
int readByContentLength(Connection &conn, std::string& bodyStr, const size_t bufferSize, const size_t maxSize)
{

    size_t bytesRead = conn.getBuffer().size();
    int ret = RECV_OK;

    conn.contentLength = std::strtol(conn.request.getHeader("Content-Length").c_str(), NULL, 10);

	if (conn.contentLength > maxSize)
		throw HttpException(PAYLOAD_TOO_LARGE, "Request Body Too Large");

    while (bytesRead < conn.contentLength) {
        ret = readFromSocket(conn, bufferSize);
        if (ret <= 0)
			return ret; // RECV_AGAIN or RECV_CLOSED

		conn.startTime = getNowInSeconds(); // reset timer
		bytesRead += ret;
	}

	if (bytesRead == conn.contentLength) {
		bodyStr = conn.getBuffer();
		conn.clearBuffer();
	}
	else { // bytesRead > conn.contentLength
        bodyStr = conn.getBuffer().substr(0, conn.contentLength);
        conn.setBuffer(conn.getBuffer().substr(conn.contentLength));
    }
    return ret;
}
int readRequestBody(Connection &conn, std::string& bodyStr, const size_t bufferSize, const size_t maxSize)
{
    std::cout << GREY << "===== readRequestBody =====" << RESET << '\n';
    int ret = RECV_OK;
    if (conn.readBodyMethod == CONTENT_LENGTH) {
        std::cout << "CONTENT_LENGTH\n";
        ret = readByContentLength(conn, bodyStr, bufferSize, maxSize);
    }
    else if (conn.readBodyMethod == CHUNKED_ENCODING) {
        std::cout << "CHUNKED ENCODING\n";
        ret = readByChunkedEncoding(conn, bodyStr, bufferSize, maxSize);
    }
    else {// NO_BODY
        std::cout << "NO BODY\n";
        return RECV_OK;
    }

    return ret;
}


