#include "../../include/sockets-polling.h"

// static int readFromSocket2(int fd, std::string& buffer)
// {
// 	char buf[BODY_BUFFER_SIZE + 1];

// 	ssize_t n = recv(fd, buf, BODY_BUFFER_SIZE, 0);

//     if (n == 0) {
// 		std::cout << "RECV_CLOSED\n";
// 		return RECV_CLOSED;
// 	}
// 	else if (n == -1) {
// 		std::cout << "RECV_AGAIN: No data available yet, will try again next iteration.\n";
// 		return RECV_AGAIN;
//     }

// 	buffer.append(buf, n);
// 	std::cout << "curBuffer: " << buffer << '\n';

// 	return n;
// }

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
	connection.appendToBuffer(buf, n);
	std::cout << "curBuffer: " << connection.getBuffer() << '\n';

	return n;
}

bool doesLineHaveCRLF(const std::string& buffer)
{
	// find "\r\n"
	size_t pos = buffer.find(CRLF, 0);
	if (pos != std::string::npos)
		return true;
	else return false;
}

size_t hexStrToSizeT(const std::string& hexStr)
{
	size_t n = 0;
	std::stringstream ss;
	ss << std::hex << hexStr;
	ss >> n;
	return n;
}

void clearStrSection(std::string& buffer, int startIndex)
{
	buffer = buffer.substr(startIndex);
}

bool isStrHex(const std::string& str)
{
	for (size_t i = 0; i < str.length(); ++i) {
		if (!std::isxdigit(str[i]))
			return false;
	}
	return true;
}

// function increments `parsePos`
size_t extractChunkSize(Connection& conn)
{
	size_t chunkSize = 0;
	// size_t pos = buffer.find(CRLF, 0);
	size_t pos = conn.findInBuffer(CRLF, 0);

	if (conn.compareBuffer(CRLF)) { // no hex digits in chunk size line
		std::cout << "Chunked Body Format Error: no hex digits in chunk size line\n";
		// 400 Bad Request
		conn.response.setStatus(BAD_REQUEST);
		throw std::logic_error("Bad request line format");

	}
	std::string sizeStr = conn.getBuffer().substr(0, pos);

	if (!isStrHex(sizeStr)) { // chunkSize string contains non-Hex characters
		std::cout << "Chunked Body Format Error: chunkSize string contains non-Hex characters\n";
		// Bad Request
		conn.response.setStatus(BAD_REQUEST);
		throw std::logic_error("Bad request line format");
	}

	chunkSize = hexStrToSizeT(sizeStr);
	std::cout << "chunkSize: " << chunkSize << '\n';

	// buffer.erase(0, sizeStr.length() + CRLF_LENGTH);
	conn.eraseBufferFromStart(sizeStr.length() + CRLF_LENGTH);
	return chunkSize;
}

std::string extractChunkData(const std::string& buffer, size_t chunkSize)
{
	std::string dataStr = buffer.substr(0, chunkSize);

	std::cout << "data: " << dataStr << '\n';

	return dataStr;
}

int readByChunkedEncoding(Connection &conn, std::string& bodyStr)
{
	int ret = RECV_OK;
	enum readChunkedRequestStatus& status = conn.readChunkedRequestStatus;

	while (status != DONE) {

		if (conn.bufferSize() == 0) {
			// ret = readFromSocket2(conn.fd, buffer);
			ret = readFromSocket(conn);
			if (ret <= 0)
				return ret; // RECV_AGAIN or RECV_CLOSED or RECV_ERROR
			conn.startTime = getNowInSeconds(); // reset timer
		}

		while (conn.bufferSize() > 0) {

			if (status == READ_CHUNK_SIZE) {
				std::cout << "READ_CHUNK_SIZE\n";
				if (!doesLineHaveCRLF(conn.getBuffer()))
					break;
				conn.chunkSize = extractChunkSize(conn);
				if (conn.chunkSize == 0)
					status = EXPECT_CRLF_AFTER_ZERO_CHUNK_SIZE;
				else
					status = READ_CHUNK_DATA;
			}
			else if (status == READ_CHUNK_DATA) {
				std::cout << "READ_CHUNK_DATA\n";

				if (conn.bufferSize() < conn.chunkSize + CRLF_LENGTH) // must have CRLF for a complete line
					break;
				std::string chunkData = extractChunkData(conn.getBuffer(), conn.chunkSize);
				// conn.appendToBuffer(chunkData.c_str(), chunkData.length());
				conn.chunkReqBuf.append(chunkData.c_str(), chunkData.length());

				if (conn.getBuffer()[conn.chunkSize] != '\r' || conn.getBuffer()[conn.chunkSize + 1] != '\n') {
					std::cout << "Chunked Body Format Error: characters after chunkData is not CRLF\n";
					// 400 Bad Request
					conn.response.setStatus(BAD_REQUEST);
					throw std::logic_error("Bad request line format");
				}

				// buffer.erase(0, conn.chunkSize + CRLF_LENGTH);
				conn.eraseBufferFromStart(conn.chunkSize + CRLF_LENGTH);
				status = READ_CHUNK_SIZE;
			}
			else if (status == EXPECT_CRLF_AFTER_ZERO_CHUNK_SIZE) {
				std::cout << "EXPECT_CRLF_AFTER_ZERO_CHUNK_SIE\n";

				if (!conn.compareBuffer(CRLF)) { // line after chunkSize 0 is not CRLF only
					std::cout << "Chunked Body Format Error: line after chunkSize 0 is not CRLF only\n";
					// 400 Bad Request
					conn.response.setStatus(BAD_REQUEST);
					throw std::logic_error("Bad request line format");
				}
				status = DONE;
				conn.eraseBufferFromStart(CRLF_LENGTH);
				// buffer.erase(0, CRLF_LENGTH);
			}
		}
	}

	std::cout << "conn.chunkReqbuf: " << conn.chunkReqBuf << '\n';
	bodyStr.append(conn.chunkReqBuf.c_str(), conn.chunkReqBuf.length());
	std::cout << "bodyStr: " << bodyStr << '\n';

	return ret;
}
