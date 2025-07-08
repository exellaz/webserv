#include "read-request.h"
#include "timeout.h"
#include <cctype>

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
    client.setReadChunkedRequestStatus(READ_CHUNK_SIZE);
    client.clearChunkReqBuf();
    client.setChunkSize(0);
    client.setFirstTimeReadingBody(true);
}

int Client::readByChunkedEncoding(std::string& bodyStr, const size_t bufferSize, const size_t maxSize)
{
    int ret = RECV_OK;
    enum readChunkedRequestStatus status = getReadChunkedRequestStatus();

    while (status != DONE) {

        if (!(isFirstTimeReadingBody() && _buffer.size() > 0)) {
            setFirstTimeReadingBody(false);

            ret = readFromSocket(bufferSize);
            if (ret <= 0)
                return ret; // RECV_AGAIN or RECV_CLOSED or RECV_ERROR
            setStartTime(getNowInSeconds()); // reset timer
        }

        while (_buffer.size() > 0) {

            if (status == READ_CHUNK_SIZE) {
                std::cout << "READ_CHUNK_SIZE\n";
                if (!doesLineHaveCRLF(getBuffer()))
                    break;
                setChunkSize(extractChunkSize(*this));
                if (getChunkSize() == 0)
                    status = EXPECT_CRLF_AFTER_ZERO_CHUNK_SIZE;
                else
                    status = READ_CHUNK_DATA;
            }
            else if (status == READ_CHUNK_DATA) {
                std::cout << "READ_CHUNK_DATA\n";

                if (_buffer.size() < getChunkSize() + CRLF_LENGTH) // must have CRLF for a complete line
                    break;
                std::string chunkData = extractChunkData(getBuffer(), getChunkSize());
                appendToChunkReqBuf(chunkData.c_str(), chunkData.length());

                if (getBuffer()[getChunkSize()] != '\r' || getBuffer()[getChunkSize() + 1] != '\n')
                    // characters after chunkData is not CRLF\n
                    throw HttpException(BAD_REQUEST, "Bad body format");

                std::cout << "chunkReqBuf size " << chunkReqBufSize() << '\n';
                if (chunkReqBufSize() > maxSize)
                    throw HttpException(PAYLOAD_TOO_LARGE, "Request Body Too Large");
                eraseBufferFromStart(getChunkSize() + CRLF_LENGTH);
                status = READ_CHUNK_SIZE;
            }
            else if (status == EXPECT_CRLF_AFTER_ZERO_CHUNK_SIZE) {
                std::cout << "EXPECT_CRLF_AFTER_ZERO_CHUNK_SIE\n";

                if (!compareBuffer(CRLF)) // line after chunkSize 0 is not CRLF only
                    throw HttpException(BAD_REQUEST, "Bad body format");
                status = DONE;
                eraseBufferFromStart(CRLF_LENGTH);
            }
        }
        setReadChunkedRequestStatus(status);
    }

    bodyStr.append(getChunkReqBuf().c_str(), chunkReqBufSize());
    resetChunkEnodingVariables(*this);

    return ret;
}
