#include "timeout.h"
#include "read-request.h"
#include "signal-handler.h"

// if actual body size is larger than contentLength, remainder is stored in buffer
int readByContentLength(Client& client, std::string& bodyStr, const size_t bufferSize, const size_t maxSize)
{

    size_t bytesRead = client.getBuffer().size();
    int ret = RECV_OK;

    client.setContentLength(std::strtol(client.request.getHeader("Content-Length").c_str(), NULL, 10));

    if (client.getContentLength() > maxSize)
        throw HttpException(PAYLOAD_TOO_LARGE, "Request Body Too Large");

    while (bytesRead < client.getContentLength()) {
        if (g_signalCaught)
            return RECV_AGAIN;
        ret = readFromSocket(client, bufferSize);
        if (ret <= 0)
            return ret; // RECV_AGAIN or RECV_CLOSED

        client.setStartTime(getNowInSeconds()); // reset timer
        bytesRead += ret;
    }

    if (bytesRead == client.getContentLength()) {
        bodyStr = client.getBuffer();
        client.clearBuffer();
    }
    else { // bytesRead > client.getContentLength()
        bodyStr = client.getBuffer().substr(0, client.getContentLength());
        client.setBuffer(client.getBuffer().substr(client.getContentLength()));
    }
    return ret;
}
int readRequestBody(Client& client, std::string& bodyStr, const size_t bufferSize, const size_t maxSize)
{
    std::cout << GREY << "===== readRequestBody =====" << RESET << '\n';
    int ret = RECV_OK;
    if (client.getReadBodyMethod() == CONTENT_LENGTH) {
        std::cout << "CONTENT_LENGTH\n";
        ret = readByContentLength(client, bodyStr, bufferSize, maxSize);
    }
    else if (client.getReadBodyMethod() == CHUNKED_ENCODING) {
        std::cout << "CHUNKED ENCODING\n";
        ret = readByChunkedEncoding(client, bodyStr, bufferSize, maxSize);
    }
    else {// NO_BODY
        std::cout << "NO BODY\n";
        return RECV_OK;
    }

    return ret;
}


