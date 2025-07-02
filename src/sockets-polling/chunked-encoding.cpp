#include "../../include/sockets-polling.h"

static bool doesLineHaveCRLF(const std::string& buffer)
{
    // find "\r\n"
    size_t pos = buffer.find(CRLF, 0);
    if (pos != std::string::npos)
        return true;
    else return false;
}

static size_t hexStrToSizeT(const std::string& hexStr)
{
    size_t n = 0;
    std::stringstream ss;
    ss << std::hex << hexStr;
    ss >> n;
    return n;
}

static bool isStrHex(const std::string& str)
{
    for (size_t i = 0; i < str.length(); ++i) {
        if (!std::isxdigit(str[i]))
            return false;
    }
    return true;
}

static size_t extractChunkSize(Client& client)
{
    size_t chunkSize = 0;
    size_t pos = client.findInBuffer(CRLF, 0);

    if (client.compareBuffer(CRLF)) // no hex digits in chunk size line
		throw HttpException(BAD_REQUEST, "Bad body format");

    std::string sizeStr = client.getBuffer().substr(0, pos);

    if (!isStrHex(sizeStr)) // chunkSize string contains non-Hex characters
        throw HttpException(BAD_REQUEST, "Bad body format");

    chunkSize = hexStrToSizeT(sizeStr);
    std::cout << "chunkSize: " << chunkSize << '\n';

    client.eraseBufferFromStart(sizeStr.length() + CRLF_LENGTH);
    return chunkSize;
}

static std::string extractChunkData(const std::string& buffer, size_t chunkSize)
{
    std::string dataStr = buffer.substr(0, chunkSize);

    std::cout << "data: " << dataStr << '\n';

    return dataStr;
}

static void resetChunkEnodingVariables(Client& client)
{
    // RESET chunkEncoding
    client.readChunkedRequestStatus = READ_CHUNK_SIZE;
    client.chunkReqBuf.clear();
    client.chunkSize = 0;
	client.isFirstTimeReadingBody = true;
}

int readByChunkedEncoding(Client& client, std::string& bodyStr, const size_t bufferSize, const size_t maxSize)
{
    int ret = RECV_OK;
    enum readChunkedRequestStatus& status = client.readChunkedRequestStatus;

    while (status != DONE) {

        if (!(client.isFirstTimeReadingBody && client.bufferSize() > 0)) {
			client.isFirstTimeReadingBody = false;

			ret = readFromSocket(client, bufferSize);
			if (ret <= 0)
				return ret; // RECV_AGAIN or RECV_CLOSED or RECV_ERROR
			client.startTime = getNowInSeconds(); // reset timer
		}

        while (client.bufferSize() > 0) {

            if (status == READ_CHUNK_SIZE) {
                std::cout << "READ_CHUNK_SIZE\n";
                if (!doesLineHaveCRLF(client.getBuffer()))
                    break;
                client.chunkSize = extractChunkSize(client);
                if (client.chunkSize == 0)
                    status = EXPECT_CRLF_AFTER_ZERO_CHUNK_SIZE;
                else
                    status = READ_CHUNK_DATA;
            }
            else if (status == READ_CHUNK_DATA) {
                std::cout << "READ_CHUNK_DATA\n";

                if (client.bufferSize() < client.chunkSize + CRLF_LENGTH) // must have CRLF for a complete line
					break;
                std::string chunkData = extractChunkData(client.getBuffer(), client.chunkSize);
                client.chunkReqBuf.append(chunkData.c_str(), chunkData.length());

				if (client.getBuffer()[client.chunkSize] != '\r' || client.getBuffer()[client.chunkSize + 1] != '\n')
					// characters after chunkData is not CRLF\n
					throw HttpException(BAD_REQUEST, "Bad body format");

                std::cout << "chunkReqBuf size " << client.chunkReqBuf.size() << '\n';
				if (client.chunkReqBuf.size() > maxSize)
				    throw HttpException(PAYLOAD_TOO_LARGE, "Request Body Too Large");
                client.eraseBufferFromStart(client.chunkSize + CRLF_LENGTH);
                status = READ_CHUNK_SIZE;
            }
            else if (status == EXPECT_CRLF_AFTER_ZERO_CHUNK_SIZE) {
                std::cout << "EXPECT_CRLF_AFTER_ZERO_CHUNK_SIE\n";

				if (!client.compareBuffer(CRLF)) // line after chunkSize 0 is not CRLF only
                    throw HttpException(BAD_REQUEST, "Bad body format");
				status = DONE;
				client.eraseBufferFromStart(CRLF_LENGTH);
			}
		}
	}

	bodyStr.append(client.chunkReqBuf.c_str(), client.chunkReqBuf.length());
	resetChunkEnodingVariables(client);

    return ret;
}
