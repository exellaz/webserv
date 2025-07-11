#include "timeout.h"
#include "client.h"
#include "signal-handler.h"

// if actual body size is larger than contentLength, remainder is stored in buffer
int Client::readByContentLength(std::string& bodyStr, const size_t bufferSize, const size_t maxSize)
{

    size_t bytesRead = _buffer.size();
    int ret = RECV_OK;

    _contentLength = std::strtol(request.getHeader("Content-Length").c_str(), NULL, 10);

    if (_contentLength > maxSize)
        throw HttpException(PAYLOAD_TOO_LARGE, "Request Body Too Large");

    while (bytesRead < _contentLength) {
        if (g_signalCaught)
            return RECV_AGAIN;
        ret = readFromSocket(bufferSize);
        if (ret <= 0)
            return ret; // RECV_AGAIN or RECV_CLOSED

        _startTime = getNowInSeconds(); // reset timer
        bytesRead += ret;
    }

    if (bytesRead == _contentLength) {
        bodyStr = _buffer;
        _buffer.clear();
    }
    else { // bytesRead > _contentLength
        bodyStr = _buffer.substr(0, _contentLength);
        _buffer = _buffer.substr(_contentLength);
    }
    return ret;
}

int Client::readRequestBody(std::string& bodyStr, const size_t bufferSize, const size_t maxSize)
{
    int ret = RECV_OK;
    if (_readBodyMethod == CONTENT_LENGTH)
        ret = readByContentLength(bodyStr, bufferSize, maxSize);
    else if (_readBodyMethod == CHUNKED_ENCODING)
        ret = readByChunkedEncoding(bodyStr, bufferSize, maxSize);
    else
        return RECV_OK;

    return ret;
}
