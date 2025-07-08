#ifndef READ_REQUEST_H
# define READ_REQUEST_H

#include <sys/socket.h>
#include <utility>
#include <map>

#include "Configuration.hpp"
#include "Client.h"
#include "color.h"

#define HEADER_END "\r\n\r\n"
#define CRLF "\r\n"
#define CRLF_LENGTH 2
#define DOUBLE_CRLF_LENGTH 4

enum readReturnVal {
    RECV_OK = 0,
    RECV_AGAIN = -1,
    RECV_CLOSED = -2,
	REQUEST_ERR = -3,
};

// int receiveClientRequest(Client& client, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers);
// int readRequestHeader(Client& client, std::string& headerStr, const size_t bufferSize);
// int readRequestBody(Client& client, std::string& bodyStr, const size_t bufferSize, const size_t maxSize);
// int readByContentLength(Client& client, std::string& bodyStr, const size_t bufferSize, const size_t maxSize);
// int readByChunkedEncoding(Client& client, std::string& bodyStr, const size_t bufferSize, const size_t maxSize);
// int readFromSocket(Client& client, int bufferSize);
// void dispatchRequest(Client& client);

#endif