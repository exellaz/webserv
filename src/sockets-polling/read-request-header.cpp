#include "../../include/sockets-polling.h"


int readFromSocket(int fd, Buffer& buf) {
	std::vector<char> buff;
    ssize_t n = recv(fd, &buff[1], buf.remainingSize(), 0);

	std::cout << "n: " << n << '\n';
	buf.last[n] = '\0';
	if (n == 0)
        return NGX_ERROR;  // connection closed
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return NGX_AGAIN;
        return NGX_ERROR;
    }
	
	buf.last += n;
    return n;
}

bool isEndOfHeaderSection(char *str, int i)
{
	if (str[i]     == '\r' &&
		str[i + 1] == '\n' &&
		str[i + 2] == '\r' &&
		str[i + 3] == '\n')
		return true;
    return false;
}

bool hasEndOfHeaderSection(const Buffer& buf) {
    size_t len = buf.last - buf.start;
    if (len < 4)
        return false;

    for (size_t i = 0; i <= len - 4; ++i) {
		if (isEndOfHeaderSection(buf.start, i))
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
		std::cout << "ret: " << ret << '\n';
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
	
	// transfer buffers without "\r\n\r\n"
	std::vector<Buffer>::iterator it = buffers.begin();
	for (; it != buffers.end() && !hasEndOfHeaderSection(*it); ++it) {
		headerStr = it->start;
	}
	
	// transfer chars from last buffer till "\r\n\r\n"
	int i = 0;
	while (it->start[i] && !isEndOfHeaderSection(it->start, i))
		++i;
	headerStr.append(it->start, i);

	return headerStr;
}

void readRequestHeader(int fd, std::string& headerStr, std::vector<Buffer>& buffers)
{
	// TODO: Handle return values/errors 
	readHeaderToBuffers(fd, buffers);

	// TODO: extractHeaderSectionFromBuffers()
	headerStr = extractHeaderSectionFromBuffers(buffers);
	
	// TODO: store leftover section -> body section in


	std::cout << headerStr << '\n';
}
