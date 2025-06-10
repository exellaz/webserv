#include "../../include/sockets-polling.h"


static int readFromSocket(Connection &connection)
{
	char buf[HEADER_BUFFER_SIZE + 1];

	ssize_t n = recv(connection.fd, buf, HEADER_BUFFER_SIZE, 0);
	std::cout << "n: " << n << '\n';
	
    if (n == 0)
        return RECV_CLOSED;
    if (n == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) // WARN: cannot use `errno` after read
            return RECV_AGAIN;
        perror("recv error");
        return RECV_ERROR;
    }

	buf[n] = '\0';
	connection.appendToBuffer(buf);

	std::cout << "current buffer:" << connection.getBuffer() << '\n';
	return n;
}

void readRequestHeader(Connection &connection, std::string& headerStr)
{
	std::cout << GREY << "===== readRequestHeader =====" << RESET << '\n';

	const std::string& buffer = connection.getBuffer();
	std::cout << "getBuffer: " << connection.getBuffer() << '\n';

	// TODO: if End if header not found -> infinite loop
	size_t found;
	while (1) {
		found = buffer.find("\r\n\r\n");
		if (found != std::string::npos)
			break;
		int ret = readFromSocket(connection);
		if (ret == RECV_OK)
			continue;
		else if (ret == RECV_AGAIN)
			return;
	}

	// if found is end index -> headerStr = Buffer
	// else -> headerstr = buffer.substr
	
	// TODO: if CRLF not found -> throw exception?
	if (found == std::string::npos) {
		std::cout << "getHeaderStr: Error: End of Header not found\n";
		/*throw BadRequestException();*/
	}
	// if no body
	if (buffer.begin() + found == buffer.end() - 4) {
		std::cout << "getHeaderStr: no body found\n";
		headerStr = buffer.substr(0, buffer.length() - 4); // exclude "abc\r\n\r\n abc"
		// buffer.clear();
		connection.clearBuffer();
	}
	else {
		headerStr = buffer.substr(0, found);
		// replace buffer with body part only
		// buffer = buffer.substr(found + 4); 
		connection.setBuffer(buffer.substr(found + 4));
	}

	std::cout << "\n===== Header String: =====\n";
	std::cout << headerStr << '\n';
	std::cout << "==========================\n\n";
	std::cout << "----Leftover in Buffer: ----\n";
	std::cout << buffer << '\n';
	std::cout << "----------------------------\n";
}

// void readRequestHeader(int fd, std::string& headerStr, std::string& buffer)
// {
// 	std::cout << GREY << "===== readRequestHeader =====" << RESET << '\n';
//
// 	// TODO: if End if header not found -> infinite loop
// 	size_t found;
// 	while (1) {
// 		found = buffer.find("\r\n\r\n");
// 		if (found != std::string::npos)
// 			break;
// 		int ret = readFromSocket(fd, buffer);
// 		if (ret == RECV_OK)
// 			continue;
// 		else if (ret == RECV_AGAIN)
// 			return;
// 	}
//
// 	// if found is end index -> headerStr = Buffer
// 	// else -> headerstr = buffer.substr
//
// 	// TODO: if CRLF not found -> throw exception?
// 	if (found == std::string::npos) {
// 		std::cout << "getHeaderStr: Error: End of Header not found\n";
// 		/*throw BadRequestException();*/
// 	}
// 	// if no body
// 	if (buffer.begin() + found == buffer.end() - 4) {
// 		std::cout << "getHeaderStr: no body found\n";
// 		headerStr = buffer.substr(0, buffer.length() - 4); // exclude "abc\r\n\r\n abc"
// 		buffer.clear();
// 	}
// 	else {
// 		headerStr = buffer.substr(0, found);
// 		// replace buffer with body part only
// 		buffer = buffer.substr(found + 4); 
// 	}
//
// 	std::cout << "\n===== Header String: =====\n";
// 	std::cout << headerStr << '\n';
// 	std::cout << "==========================\n\n";
// 	std::cout << "----Leftover in Buffer: ----\n";
// 	std::cout << buffer << '\n';
// 	std::cout << "----------------------------\n";
// }



// int readFromSocket(int fd, Buffer& buf) {
// 	std::cout << "remainingSize: " << buf.remainingSize() << '\n';
//     ssize_t n = recv(fd, buf.last(), buf.remainingSize(), 0);
// 	std::cout << "n: " << n << '\n';
//
// 	if (n == 0)
//         return NGX_ERROR;  // connection closed
//     if (n < 0) {
//         if (errno == EAGAIN || errno == EWOULDBLOCK) // WARN: cannot use errno
//             return NGX_AGAIN;
//         return NGX_ERROR;
//     }
// 	buf.lastIndex += n;
// 	// buf.data[buf.lastIndex -1] = '\0';
//
//     return n;
// }
//
//
// bool isEndOfHeaderSection(const std::vector<char> &str, int i)
// {
// 	if (str[i]     == '\r' &&
// 		str[i + 1] == '\n' &&
// 		str[i + 2] == '\r' &&
// 		str[i + 3] == '\n')
// 		return true;
//     return false;
// }
//
// bool hasEndOfHeaderSection(Buffer& buf) {
//     size_t len = buf.dataSize();
//
//     if (len < 4)
//         return false;
//
//     for (size_t i = 0; i <= len - 4; ++i) {
// 		if (isEndOfHeaderSection(buf.data, i))
// 			return true;
//     }
//     return false;
// }
//
//
// int readHeaderToBuffers(int fd, std::vector<Buffer>& buffers) {
//     if (buffers.empty())
//         buffers.push_back(Buffer());
//
//     while (1) {
//         Buffer& current = buffers.back();
//
//         int ret = readFromSocket(fd, current);
//         if (ret == NGX_AGAIN)
//             return NGX_AGAIN;
//         if (ret == NGX_ERROR)
//             return NGX_ERROR;
//         if (hasEndOfHeaderSection(current))
// 			return NGX_OK;
//
//         if (current.remainingSize() == 0) {
//             // buffer is full — try allocating a large buffer
//             size_t largeBufferUsed = buffers.size() - 1;  // first buffer is client_header_buffer_size
//             if (largeBufferUsed < MAX_LARGE_BUFFERS) {
//                 buffers.push_back(Buffer(LARGE_HEADER_BUFFER_SIZE));
//             } else {
//                 return NGX_REQUEST_HEADER_TOO_LARGE;
//             }
//         }
//     }
//
// }
//
//
// std::string extractHeaderSectionFromBuffers(std::vector<Buffer>& buffers)
// {
// 	std::string headerStr;
//
// 	std::vector<Buffer>::iterator it = buffers.begin();
// 	for (; it != buffers.end(); ++it) {
//         const std::vector<char>& data = it->data;
//
// 		if (hasEndOfHeaderSection(*it)) {
// 			int i = 0;
// 			while (!isEndOfHeaderSection(it->data, i))
// 				++i;
// 			headerStr.append(it->data.begin(),	it->data.begin() + i);
// 			break;
// 		}
//
// 		// transfer buffers without "\r\n\r\n"
// 		headerStr.append(data.begin(), data.begin() + it->dataSize());
// 	}
//
// 	return headerStr;
// }
//
// void storeLeftoverBuffer(std::vector<Buffer>& buffers) 
// {
// 	if (!hasEndOfHeaderSection(buffers.back()))
// 		return ;
//
// 	const std::vector<char>& lastBuffer = buffers.back().data;
// 	int i = 0;
// 	while (!isEndOfHeaderSection(lastBuffer, i))
// 		++i;
// 	// std::string leftover = &lastBuffer[i + 4];
//
// 	std::string leftover(lastBuffer.begin() + i + 4, lastBuffer.end());
//
// 	// Replaces entire of `buffers` with section after header
// 	buffers.clear();
// 	buffers.push_back(Buffer(leftover));
//
// }
//
// void readRequestHeader(int fd, std::string& headerStr, std::vector<Buffer>& buffers)
// {
// 	// TODO: Handle return values/errors 
// 	readHeaderToBuffers(fd, buffers);
//
// 	headerStr = extractHeaderSectionFromBuffers(buffers);
// 	std::cout << "\n===== Header String: =====\n";
// 	std::cout << headerStr << '\n';
// 	std::cout << "==========================\n\n";
//
// 	// TODO: store leftover section -> body section in
// 	storeLeftoverBuffer(buffers);
// 	std::cout << "leftover: " << &(buffers[0].data[0]) << '\n';	
// }
