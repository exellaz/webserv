#include "../../include/sockets-polling.h"

int readRequestHeader(Connection &conn, std::string& headerStr, const size_t bufferSize)
{
    std::cout << GREY << "===== readRequestHeader =====" << RESET << '\n';

    size_t found;
    int ret;

    while (1) {

        ret = readFromSocket(conn, bufferSize);
        if (ret > 0) {
            conn.startTime = getNowInSeconds(); // reset timer
            found = conn.getBuffer().find(HEADER_END);
            if (found != std::string::npos)
                break;
            else
                continue;
        }
        else
            return ret; // RECV_AGAIN or RECV_CLOSED or RECV_ERROR
    }


    const std::string& buffer = conn.getBuffer();

    // TODO: if CRLF not found -> throw exception?
    if (found == std::string::npos) {
        std::cout << "getHeaderStr: Error: End of Header not found\n";
        throw HttpException(BAD_REQUEST, "No header found");
    }
    // if no body
    if (buffer.begin() + found == buffer.end() - 4) {
        std::cout << "getHeaderStr: no body found\n";
        headerStr = buffer.substr(0, buffer.length() - 4); // exclude "abc\r\n\r\n abc"
        conn.clearBuffer();
    }
    else {
        headerStr = buffer.substr(0, found);
        // replace buffer with body part only
        conn.setBuffer(buffer.substr(found + 4));
    }

    // std::cout << "\n===== Header String: =====\n";
    // std::cout << headerStr << '\n';
    // std::cout << "==========================\n\n";
    // std::cout << "----Leftover in Buffer: ----\n";
    // std::cout << buffer << '\n';
    // std::cout << "----------------------------\n";
    return ret;
}

