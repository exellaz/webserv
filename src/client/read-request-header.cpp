#include "timeout.h"
#include "Client.h"

int Client::readRequestHeader(std::string& headerStr, const size_t bufferSize)
{
    size_t found;
    int ret;

    while (1) {

        ret = readFromSocket(bufferSize);
        if (ret > 0) {
            _startTime = getNowInSeconds(); // reset timer
            found = _buffer.find(HEADER_END);
            if (found != std::string::npos)
                break;
            else
                continue;
        }
        else
            return ret; // RECV_AGAIN or RECV_CLOSED
    }

    const std::string& buffer = _buffer;

    if (found == std::string::npos)
        throw HttpException(BAD_REQUEST, "No header found");

    // if no body
    if (buffer.begin() + found == buffer.end() - DOUBLE_CRLF_LENGTH) {
        headerStr = buffer.substr(0, buffer.length() - DOUBLE_CRLF_LENGTH);
        _buffer.clear();
    }
    else {
        headerStr = buffer.substr(0, found);
        _buffer = buffer.substr(found + DOUBLE_CRLF_LENGTH);
    }

    return ret;
}
