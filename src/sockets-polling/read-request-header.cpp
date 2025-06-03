#include "../../include/sockets-polling.h"


// int readFromSocket(int fd, Buffer& buf) {
//     ssize_t n = recv(fd, buf.last, buf.remainingSize(), 0);
//
// 	std::cout << "n: " << n << '\n';
// 	buf.last[n] = '\0';
// 	if (n == 0)
//         return NGX_ERROR;  // connection closed
//     if (n < 0) {
//         if (errno == EAGAIN || errno == EWOULDBLOCK)
//             return NGX_AGAIN;
//         return NGX_ERROR;
//     }
//
// 	buf.last += n;
//     return n;
// }

int readFromSocket(int fd, Buffer& buf) {
    ssize_t n = recv(fd, buf.last(), buf.remainingSize(), 0);

	buf.lastIndex += n;
	buf.data[buf.lastIndex] = '\0';
	
	if (n == 0)
        return NGX_ERROR;  // connection closed
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return NGX_AGAIN;
        return NGX_ERROR;
    }
	
    return n;
}

bool isEndOfHeaderSection(std::vector<char> &str, int i)
{
	if (str[i]     == '\r' &&
		str[i + 1] == '\n' &&
		str[i + 2] == '\r' &&
		str[i + 3] == '\n')
		return true;
    return false;
}

bool hasEndOfHeaderSection(Buffer& buf) {
    size_t len = buf.dataSize();

    if (len < 4)
        return false;

    for (size_t i = 0; i <= len - 4; ++i) {
		if (isEndOfHeaderSection(buf.data, i))
			return true;
    }
    return false;
}


int readHeaderToBuffers(int fd, std::vector<Buffer>& buffers) {
    if (buffers.empty())
        buffers.push_back(Buffer(CLIENT_HEADER_BUFFER_SIZE));

    while (1) {
        Buffer& current = buffers.back();

        int ret = readFromSocket(fd, current);
        if (ret == NGX_AGAIN)
            return NGX_AGAIN;
        if (ret == NGX_ERROR)
            return NGX_ERROR;
        if (hasEndOfHeaderSection(current))
			return NGX_OK;

        if (current.remainingSize() == 0) {
            // buffer is full — try allocating a large buffer
            size_t largeBufferUsed = buffers.size() - 1;  // first buffer is client_header_buffer_size
            if (largeBufferUsed < MAX_LARGE_BUFFERS) {
                buffers.push_back(Buffer(LARGE_HEADER_BUFFER_SIZE));
            } else {
                return NGX_REQUEST_HEADER_TOO_LARGE;
            }
        }
    }
	
}


std::string extractHeaderSectionFromBuffers(std::vector<Buffer>& buffers)
{
	std::string headerStr;
	
	std::vector<Buffer>::iterator it = buffers.begin();
	for (; it != buffers.end(); ++it) {
        const std::vector<char>& data = it->data;
        size_t len = it->dataSize();

		if (hasEndOfHeaderSection(*it)) {
			int i = 0;
			while (!isEndOfHeaderSection(it->data, i))
				++i;
			headerStr.append(it->data.begin(),	it->data.begin() + i);
			break;
		}

		// transfer buffers without "\r\n\r\n"
		headerStr.append(data.begin(), data.begin() + len);
	}
	
	return headerStr;
}

// NOTE: Replaces entire of `buffers` with section after header
void storeLeftoverBuffer(std::vector<Buffer>& buffers) 
{
	// TODO: extract leftover to std::string
	
	// TODO: clear list
	
	// TODO: add new buffer with leftover_string to `buffers` 

}

void readRequestHeader(int fd, std::string& headerStr, std::vector<Buffer>& buffers)
{
	// TODO: Handle return values/errors 
	readHeaderToBuffers(fd, buffers);

	headerStr = extractHeaderSectionFromBuffers(buffers);
	
	// TODO: store leftover section -> body section in


	std::cout << "\n===== Header String: =====\n";
	std::cout << headerStr << '\n';
	std::cout << "==========================\n\n";
}
