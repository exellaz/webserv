#include "../../include/sockets-polling.h"

static int readFromSocket2(int fd, std::string& buffer)
{
	char buf[BODY_BUFFER_SIZE + 1];

	ssize_t n = recv(fd, buf, BODY_BUFFER_SIZE, 0);
	std::cout << "n: " << n << '\n';

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

bool isLineComplete(const std::string& buffer, size_t parsePos)
{
	// find "\r\n"
	size_t pos = buffer.find(CRLF, parsePos);
	if (pos != std::string::npos)
		return true;
	else return false;
}

/*
 
- extract substr before "\r\n"
- convert substr => size_t

*/
size_t extractChunkSize(std::string& buffer, size_t parsePos)
{
	size_t chunkSize = 0;
	size_t pos = buffer.find(CRLF, parsePos);

	std::string sizeStr = buffer.substr(parsePos, pos);
	std::cout << "sizeStr: " << sizeStr << '\n';
	
	
	return chunkSize;
}

int readByChunkedEncoding(Connection &conn, std::string& bodyStr)
{
	(void)bodyStr;
	int ret = RECV_OK; 
	enum decodingChunkedRequestStatus status = READ_CHUNK_SIZE;
	std::string buffer;
	size_t parsePos = 0;

	while (status != DONE) {
		
		ret = readFromSocket2(conn.fd, buffer);
		if (ret <= 0)
			return ret; // RECV_AGAIN or RECV_CLOSED or RECV_ERROR
		conn.startTime = getNowInSeconds(); // reset timer

		while (parsePos < buffer.size()) {
			
			switch (status) {
				case READ_CHUNK_SIZE:
					if (isLineComplete(buffer, parsePos)) {
						int chunkSize = extractChunkSize(buffer, parsePos);
						// TODO: increment `parsePos`
						if (chunkSize == 0)
							status = DONE;
						else 
							status = READ_CHUNK_DATA;
					}
					break;

				case READ_CHUNK_DATA:
					if (isLineComplete(buffer, parsePos)) {

						// TODO: increment `parsePos`
					}


					break;
				case EXPECT_CRLF_AFTER_DATA:

					break;
				case DONE:

					break;
			}
		}
	}


	return ret;
}
