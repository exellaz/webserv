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

static size_t extractChunkSize(Connection& conn)
{
    size_t chunkSize = 0;
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

    conn.eraseBufferFromStart(sizeStr.length() + CRLF_LENGTH);
    return chunkSize;
}

static std::string extractChunkData(const std::string& buffer, size_t chunkSize)
{
    std::string dataStr = buffer.substr(0, chunkSize);

    std::cout << "data: " << dataStr << '\n';

    return dataStr;
}

static void resetChunkEnodingVariables(Connection &conn)
{
    // RESET chunkEncoding
    conn.readChunkedRequestStatus = READ_CHUNK_SIZE;
    conn.chunkReqBuf.clear();
    conn.chunkSize = 0;
}

int readByChunkedEncoding(Connection &conn, std::string& bodyStr, int bufferSize)
{
    int ret = RECV_OK;
    enum readChunkedRequestStatus& status = conn.readChunkedRequestStatus;

    while (status != DONE) {

        if (conn.bufferSize() == 0) {
            ret = readFromSocket(conn, bufferSize);
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
                conn.chunkReqBuf.append(chunkData.c_str(), chunkData.length());

                if (conn.getBuffer()[conn.chunkSize] != '\r' || conn.getBuffer()[conn.chunkSize + 1] != '\n') {
                    std::cout << "Chunked Body Format Error: characters after chunkData is not CRLF\n";
                    // 400 Bad Request
                    conn.response.setStatus(BAD_REQUEST);
                    throw std::logic_error("Bad request line format");
                }

				if (conn.chunkReqBuf.size() > CLIENT_MAX_BODY_SIZE)
				    throw HttpException(PAYLOAD_TOO_LARGE, "Request Body Too Large");
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
            }
        }
    }

    bodyStr.append(conn.chunkReqBuf.c_str(), conn.chunkReqBuf.length());
    resetChunkEnodingVariables(conn);

    return ret;
}
