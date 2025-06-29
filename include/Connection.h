
#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include "http-request.h"
#include "http-response.h"
#include "http-exception.h"
#include "Configuration.hpp"
#include <iostream>
#include <sys/time.h>

enum ConnState {
	ACTIVE,
	DISCONNECTED
};

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

	const int fd;
	enum ConnState connState;
	time_t startTime; // Timeout

	enum connectionType connType;
	enum readBodyMethod readBodyMethod;

	size_t contentLength;

	// Chunked Encoding
	enum readChunkedRequestStatus readChunkedRequestStatus;
	size_t chunkSize;
	std::string chunkReqBuf;
	bool isFirstTimeReadingBody;
	bool isResponseReady;

	HttpRequest request;
	HttpResponse response;
	Server server;
	Location location;

	std::string locationPath;
	bool isJustLocationPath;

	// Buffer methods
	void appendToBuffer(const char *str, size_t n);
	const std::string& getBuffer() const;
	void setBuffer(std::string str);
	void clearBuffer();
	void eraseBufferFromStart(size_t n);
	size_t bufferSize() const;
	bool compareBuffer(const std::string str);
	size_t findInBuffer(const std::string str, size_t pos);
	// void resolveServerConfig(std::vector<Config>& configs, HttpRequest& request);

	void assignServerByServerName(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
									std::pair<std::string, std::string> ipPort, Server& defaultServer);


private:
	std::string _buffer;
};

std::ostream & operator<<( std::ostream & o, Connection const & connection );
void dispatchRequest(Connection& connection);
int handleParsingError(const HttpException& e, HttpResponse& response, Connection& connection);


#endif
