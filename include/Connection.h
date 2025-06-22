
#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include "http-request.h"
#include "http-response.h"
#include <iostream>
#include <sys/time.h>

enum connectionType {
	CLOSE,
	KEEP_ALIVE,
};

enum readBodyMethod {
	CONTENT_LENGTH,
	CHUNKED_ENCODING,
	NO_BODY,
};

enum readChunkedRequestStatus {
	READ_CHUNK_SIZE,
	READ_CHUNK_DATA,
	EXPECT_CRLF_AFTER_ZERO_CHUNK_SIZE,
	DONE
};

class Connection {
public:
	// Constructor
	Connection(int fd, time_t startTime);
	// Copy Constructor
	Connection(const Connection& other);
	// Copy Assignment Operator
	Connection& operator=(const Connection& other);
	// Destructor
	~Connection();

	int fd;
	time_t startTime; // Timeout
	enum connectionType connType;
	enum readBodyMethod readBodyMethod;
	size_t contentLength;
	enum readChunkedRequestStatus readChunkedRequestStatus;
	size_t chunkSize;
	std::string chunkReqBuf;
	bool isResponseReady;

	void appendToBuffer(const char *str, size_t n);
	const std::string& getBuffer() const;
	void setBuffer(std::string str);
	void clearBuffer();

	HttpRequest request;
	HttpResponse response;

private:
	std::string _buffer;
};

std::ostream & operator<<( std::ostream & o, Connection const & connection );

#endif
