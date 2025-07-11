#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <iostream>
#include <sys/time.h>

#include "http-request.h"
#include "Configuration.hpp"
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
	Server server;
	Location location;

	// getters
	int getFd() const;
	enum ConnState getConnState() const;
	time_t getStartTime() const;
	enum connectionType getConnType() const;
	const std::string& getLocationPath() const;
	const std::string& getSessionId() const;
	const std::string& getSessionData() const;

	// setters
	void setFd(int fd);
	void setConnState(enum ConnState connState);
	void setConnType(enum connectionType connType);
	void setLocationPath(std::string locationPath);
	void setSessionId(const std::string& sessionId);
	void setSessionData(const std::string& sesionData);

	// Buffer methods
	const std::string& getBuffer() const;
	void clearBuffer(); // neexed

	void assignServerByServerName(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
									std::pair<std::string, std::string> ipPort, Server& defaultServer);

	// read-request
	int receiveClientRequest(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers);
	int readRequestHeader(std::string& headerStr, const size_t bufferSize);
	int readRequestBody(std::string& bodyStr, const size_t bufferSize, const size_t maxSize);
	int readByContentLength(std::string& bodyStr, const size_t bufferSize, const size_t maxSize);
	int readByChunkedEncoding(std::string& bodyStr, const size_t bufferSize, const size_t maxSize);
	size_t extractChunkSize();
	std::string extractChunkData();
	void resetChunkEnodingVariables();
	int readFromSocket(int bufferSize);
	void dispatchRequest();
	void sendResponseToClient();

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
	bool _firstTimeReadingBody;

	std::string _locationPath;
	std::string _sessionId;
	std::string _sessionData;
};

std::ostream & operator<<( std::ostream & o, Client const & client );

#endif
