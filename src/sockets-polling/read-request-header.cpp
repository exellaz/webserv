#include "../../include/Buffer.h"


int readFromSocket(int fd, Buffer& buf) {
    ssize_t n = recv(fd, buf.last, buf.remainingSize(), 0);
if (n == 0) {
        return NGX_ERROR;  // connection closed
    }
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return NGX_AGAIN;
        return NGX_ERROR;
    }
	
    buf.last += n;
    return n;
}

bool headerSectionComplete(const Buffer& buf) {
    size_t len = buf.last - buf.start;
    if (len < 4)
        return false;

    for (size_t i = 0; i <= len - 4; ++i) {
        if (buf.start[i]     == '\r' &&
            buf.start[i + 1] == '\n' &&
            buf.start[i + 2] == '\r' &&
            buf.start[i + 3] == '\n') {

			buf.start[len] = '\0'; // NOTE: null-terminate
            return true;
        }
    }
    return false;
}


int readHeaderToBuffers(int fd, std::vector<Buffer>& buffers) {
    if (buffers.empty())
        buffers.push_back(Buffer(CLIENT_HEADER_BUFFER_SIZE));

    while (1) {
        Buffer& current = buffers.back();

        int n = readFromSocket(fd, current);

        if (n == NGX_AGAIN)
            return NGX_AGAIN;
        if (n == NGX_ERROR)
            return NGX_ERROR;
        if (headerSectionComplete(current))
            return NGX_OK;

        if (current.remainingSize() == 0) {
            // buffer is full — try allocating a large buffer
            size_t large_buffers_used = buffers.size() - 1;  // first buffer is client_header_buffer_size
            if (large_buffers_used < MAX_LARGE_BUFFERS) {
                buffers.push_back(Buffer(LARGE_HEADER_BUFFER_SIZE));
            } else {
                return NGX_REQUEST_HEADER_TOO_LARGE;
            }
        }
    }
}

std::string combineBuffersToSingleString(std::vector<Buffer>& buffers)
{
	std::string str;

	std::vector<Buffer>::iterator it = buffers.begin();
	
	for (; it != buffers.end(); ++it) {
		str = it->start;		
	}
	return str;
}

void readRequestHeader(int fd, std::string& headerStr, std::vector<Buffer>& buffers)
{

	readHeaderToBuffers(fd, buffers);

	// TODO: Convert buffers -> single string
	headerStr = combineBuffersToSingleString(buffers);

	std::cout << headerStr << '\n';
}
