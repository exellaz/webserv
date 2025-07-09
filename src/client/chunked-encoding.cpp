#include "timeout.h"
#include <cctype>
#include "Client.h"

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

size_t Client::extractChunkSize()
{
    size_t chunkSize = 0;
    size_t pos = _buffer.find(CRLF, 0);

    if (_buffer == CRLF) // no hex digits in chunk size line
        throw HttpException(BAD_REQUEST, "Bad body format");

    std::string sizeStr = _buffer.substr(0, pos);

    if (!isStrHex(sizeStr)) // chunkSize string contains non-Hex characters
        throw HttpException(BAD_REQUEST, "Bad body format");

    chunkSize = hexStrToSizeT(sizeStr);
    // std::cout << "chunkSize: " << chunkSize << '\n';
    _buffer.erase(0, sizeStr.length() + CRLF_LENGTH);

    return chunkSize;
}

std::string Client::extractChunkData()
{
    std::string dataStr = _buffer.substr(0, _chunkSize);

    // std::cout << "data: " << dataStr << '\n';

    return dataStr;
}

void Client::resetChunkEnodingVariables()
{
    // RESET chunkEncoding
    _readChunkedRequestStatus = READ_CHUNK_SIZE;
    _chunkReqBuf.clear();
    _chunkSize = 0;
    _firstTimeReadingBody = true;
}

int Client::readByChunkedEncoding(std::string& bodyStr, const size_t bufferSize, const size_t maxSize)
{
    int ret = RECV_OK;
    enum readChunkedRequestStatus status = _readChunkedRequestStatus;

    while (status != DONE) {

        if (!(_firstTimeReadingBody && _buffer.size() > 0)) {
            _firstTimeReadingBody = false;

            ret = readFromSocket(bufferSize);
            if (ret <= 0)
                return ret; // RECV_AGAIN or RECV_CLOSED or RECV_ERROR
            _startTime = getNowInSeconds(); // reset timer
        }

        while (_buffer.size() > 0) {

            if (status == READ_CHUNK_SIZE) {
                std::cout << "READ_CHUNK_SIZE\n";
                if (!doesLineHaveCRLF(_buffer))
                    break;
                _chunkSize = extractChunkSize();
                if (_chunkSize == 0)
                    status = EXPECT_CRLF_AFTER_ZERO_CHUNK_SIZE;
                else
                    status = READ_CHUNK_DATA;
            }
            else if (status == READ_CHUNK_DATA) {
                std::cout << "READ_CHUNK_DATA\n";

                if (_buffer.size() < _chunkSize + CRLF_LENGTH) // must have CRLF for a complete line
                    break;
                std::string chunkData = extractChunkData();
                _chunkReqBuf.append(chunkData.c_str(), chunkData.length());

                if (_buffer[_chunkSize] != '\r' || _buffer[_chunkSize + 1] != '\n')
                    // characters after chunkData is not CRLF\n
                    throw HttpException(BAD_REQUEST, "Bad body format");

                std::cout << "chunkReqBuf size " << _chunkReqBuf.size() << '\n';
                if (_chunkReqBuf.size() > maxSize)
                    throw HttpException(PAYLOAD_TOO_LARGE, "Request Body Too Large");
                _buffer.erase(0, _chunkSize + CRLF_LENGTH);

                status = READ_CHUNK_SIZE;
            }
            else if (status == EXPECT_CRLF_AFTER_ZERO_CHUNK_SIZE) {
                std::cout << "EXPECT_CRLF_AFTER_ZERO_CHUNK_SIE\n";

                if (_buffer != CRLF) // line after chunkSize 0 is not CRLF only
                    throw HttpException(BAD_REQUEST, "Bad body format");
                status = DONE;
                _buffer.erase(0, CRLF_LENGTH);

            }
        }
        _readChunkedRequestStatus = status;
    }

    bodyStr.append(_chunkReqBuf.c_str(), _chunkReqBuf.size());
    resetChunkEnodingVariables();

    return ret;
}
