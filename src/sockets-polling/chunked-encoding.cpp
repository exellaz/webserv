#include "../../include/sockets-polling.h"

static int readFromSocket2(int fd, std::string& buffer)
{
	char buf[BODY_BUFFER_SIZE + 1];

	ssize_t n = recv(fd, buf, BODY_BUFFER_SIZE, 0);

    if (n == 0) {
		std::cout << "RECV_CLOSED\n";
		return RECV_CLOSED;
	}
	else if (n == -1) {
		std::cout << "RECV_AGAIN: No data available yet, will try again next iteration.\n";
		return RECV_AGAIN;
    }

	buffer.append(buf, n);
	std::cout << "curBuffer: " << buffer << '\n';

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
size_t extractChunkSize(std::string& buffer)
{
	size_t chunkSize = 0;
	size_t pos = buffer.find(CRLF, 0);
	// TODO: if CRLF not found

	if (pos == 0) {
		// TODO: if there's no "0"
		std::cout << "no '0' in last chunk\n";
	}
	std::string sizeStr = buffer.substr(0, pos);

	if (!isStrHex(sizeStr)) {
		// TODO: 400 Bad Request
		std::cout << "chunkSize string contains non-Hex characters\n";
	}

	chunkSize = hexStrToSizeT(sizeStr);
	std::cout << "chunkSize: " << chunkSize << '\n';

	buffer.erase(0, sizeStr.length() + CRLF_LENGTH);
	return chunkSize;
}

std::string extractChunkData(std::string& buffer, size_t chunkSize)
{
	std::string dataStr = buffer.substr(0, chunkSize);

	std::cout << "data: " << dataStr << '\n';

	buffer.erase(0, dataStr.length() + CRLF_LENGTH);
	return dataStr;
}

int readByChunkedEncoding(Connection &conn, std::string& bodyStr)
{
	(void)bodyStr;
	int ret = RECV_OK;
	enum readChunkedRequestStatus& status = conn.readChunkedRequestStatus;
	std::string& buffer = conn.chunkReqBuf;

	while (status != DONE) {

		ret = readFromSocket2(conn.fd, buffer);
		if (ret <= 0)
			return ret; // RECV_AGAIN or RECV_CLOSED or RECV_ERROR
		conn.startTime = getNowInSeconds(); // reset timer

		while (buffer.size() > 0) {

			if (buffer == "\r\n") {
				std::cout << "Invalid Case: line with just CRLF\n";
				// TODO: 400 Bad Request
			}
			if (status == READ_CHUNK_SIZE) {
				std::cout << "READ_CHUNK_SIZE\n";
				if (!doesLineHaveCRLF(buffer))
					break;
				conn.chunkSize = extractChunkSize(buffer);
				if (conn.chunkSize == 0)
					status = EXPECT_CRLF_AFTER_ZERO_CHUNK_SIZE;
				else
					status = READ_CHUNK_DATA;
			}
			else if (status == READ_CHUNK_DATA) {
				std::cout << "READ_CHUNK_DATA\n";

				if (buffer.size() < conn.chunkSize + CRLF_LENGTH) // must have CRLF for a complete line
					break;
				std::string chunkData = extractChunkData(buffer, conn.chunkSize);
				conn.appendToBuffer(chunkData.c_str(), chunkData.length());

				// Now verify CRLF
				if (buffer[conn.chunkSize] != '\r' || buffer[conn.chunkSize + 1] != '\n') { // Malformed request
					// TODO: 400 Bad Request
				}

				buffer.erase(0, conn.chunkSize + CRLF_LENGTH);
				status = READ_CHUNK_SIZE;
			}
			else if (status == EXPECT_CRLF_AFTER_ZERO_CHUNK_SIZE) {
				std::cout << "EXPECT_CRLF_AFTER_ZERO_CHUNK_SIE\n";

				if (buffer != "\r\n") {
					std::cout << "Invalid Case: line after chunkSize 0 is not CRLF only\n";
					// TODO: 400 Bad Request
				}
				status = DONE;
				buffer.erase(0, CRLF_LENGTH);
			}
		}
	}


	return ret;
}
