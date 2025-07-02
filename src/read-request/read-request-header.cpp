#include "timeout.h"
#include "read-request.h"

int readRequestHeader(Client& client, std::string& headerStr, const size_t bufferSize)
{
    std::cout << GREY << "===== readRequestHeader =====" << RESET << '\n';

    size_t found;
    int ret;

    while (1) {

        ret = readFromSocket(client, bufferSize);
        if (ret > 0) {
            client.startTime = getNowInSeconds(); // reset timer
            found = client.getBuffer().find(HEADER_END);
            if (found != std::string::npos)
                break;
            else
                continue;
        }
        else
            return ret; // RECV_AGAIN or RECV_CLOSED
    }

    const std::string& buffer = client.getBuffer();

    // TODO: if CRLF not found -> throw exception?
    if (found == std::string::npos) {
        std::cout << "getHeaderStr: Error: End of Header not found\n";
        throw HttpException(BAD_REQUEST, "No header found");
    }
    // if no body
    if (buffer.begin() + found == buffer.end() - DOUBLE_CRLF_LENGTH) {
        std::cout << "getHeaderStr: no body found\n";
        headerStr = buffer.substr(0, buffer.length() - DOUBLE_CRLF_LENGTH);
        client.clearBuffer();
    }
    else {
        headerStr = buffer.substr(0, found);
        // replace buffer with body part only
        client.setBuffer(buffer.substr(found + DOUBLE_CRLF_LENGTH));
    }

    return ret;
}

