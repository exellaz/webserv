#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "http-request.h"
#include "http-response.h"
#include "http-exception.h"
#include "Configuration.hpp"
#include <iostream>
#include <sys/time.h>
#include "color.h"

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

class Client {
public:
	// Constructor
	Client(int fd, time_t startTime);
	// Copy Constructor
	Client(const Client& other);
	// Copy Assignment Operator
	Client& operator=(const Client& other);
	// Destructor
	~Client();

	HttpRequest request;
	HttpResponse response;
	Server& server;
	Location& location;

	// getters
	int getFd() const;
	enum ConnState getConnState() const;
	time_t getStartTime() const;
	enum connectionType getConnType() const;
	enum readBodyMethod getReadBodyMethod() const;
	size_t getContentLength() const;
	enum readChunkedRequestStatus getReadChunkedRequestStatus() const;
	size_t getChunkSize() const;
	const std::string& getLocationPath() const;
	bool isFirstTimeReadingBody() const;
	bool isResponseReady() const;

	// setters
	void setFd(int fd);
	void setConnState(enum ConnState connState);
	void setStartTime(time_t startTime);
	void setConnType(enum connectionType connType);
	void setReadBodyMethod(enum readBodyMethod readBodyMethod);
	void setContentLength(size_t contentLength);
	void setReadChunkedRequestStatus(enum readChunkedRequestStatus chunkedRequestStatus);
	void setChunkSize(size_t chunkedSize);
	void setLocationPath(std::string locationPath);
	void setFirstTimeReadingBody(bool status);
	void setResponseReady(bool status);

	// Buffer methods
	void appendToBuffer(const char *str, size_t n);
	const std::string& getBuffer() const;
	void setBuffer(std::string str);
	void clearBuffer();
	void eraseBufferFromStart(size_t n);
	size_t bufferSize() const;
	bool compareBuffer(const std::string str);
	size_t findInBuffer(const std::string str, size_t pos);
	
	// chunkReqBuf methods
	void appendToChunkReqBuf(const char *str, size_t n);
	const std::string& getChunkReqBuf() const;
	void setChunkReqBuf(std::string str);
	void clearChunkReqBuf();
	void eraseChunkReqBufFromStart(size_t n);
	size_t chunkReqBufSize() const;
	bool compareChunkReqBuf(const std::string str);
	size_t findInChunkReqBuf(const std::string str, size_t pos);

	void assignServerByServerName(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
									std::pair<std::string, std::string> ipPort, Server& defaultServer);


private:
	int _fd;
	enum ConnState _connState;
	time_t _startTime; // Timeout
	enum connectionType _connType;
	std::string _buffer;
	enum readBodyMethod _readBodyMethod;
	size_t _contentLength;
	enum readChunkedRequestStatus _readChunkedRequestStatus;
	size_t _chunkSize;
	std::string _chunkReqBuf;
	std::string _locationPath;
	bool _firstTimeReadingBody;
	bool _responseReady;
};

std::ostream & operator<<( std::ostream & o, Client const & client );

#endif
