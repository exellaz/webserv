#include "../../include/Client.h"

Client::Client(int fd, time_t startTime) :
    request(), response(), server(), location(),
    _fd(fd), _connState(ACTIVE), _startTime(startTime), _connType(KEEP_ALIVE),
    _buffer(""), _readBodyMethod(CONTENT_LENGTH), _contentLength(0),
    _readChunkedRequestStatus(READ_CHUNK_SIZE), _chunkSize(0),
    _chunkReqBuf(""), _firstTimeReadingBody(true),
    _locationPath(""), _sessionId(""), _sessionData("")

{
    // std::cout << "Client:: Constructor Called (name: " << index << ")" << std::endl;


}

Client::Client(const Client& other) :
    request(other.request), response(other.response), server(other.server), location(other.location),
    _fd(other._fd), _connState(other._connState), _startTime(other._startTime), _connType(other._connType),
    _buffer(other._buffer), _readBodyMethod(other._readBodyMethod), _contentLength(other._contentLength),
    _readChunkedRequestStatus(other._readChunkedRequestStatus), _chunkSize(other._chunkSize),
    _chunkReqBuf(other._chunkReqBuf), _firstTimeReadingBody(other._firstTimeReadingBody),
    _locationPath(other._locationPath), _sessionId(other._sessionId), _sessionData(other._sessionData)

{
    // std::cout << "Client:: Copy Constructor Called" << std::endl;
}

Client& Client::operator=(const Client& other)
{
    // std::cout << "Client:: Copy Assignment Operator Called" << std::endl;

    if (this == &other)
        return *this;

    request                  = other.request;
    response                 = other.response;
    server                   = other.server;
    location                 = other.location;
    _fd						 = other._fd;
    _connState 				 = other._connState;
    _startTime 				 = other._startTime;
    _connType 				 = other._connType;
    _buffer 				 = other._buffer;
    _readBodyMethod 		 = other._readBodyMethod;
    _contentLength 			 = other._contentLength;
    _readChunkedRequestStatus = other._readChunkedRequestStatus;
    _chunkSize 				 = other._chunkSize;
    _chunkReqBuf 			 = other._chunkReqBuf;
    _firstTimeReadingBody 	 = other._firstTimeReadingBody;
    _locationPath			 = other._locationPath;
    _sessionId               = other._sessionId;
    _sessionData             = other._sessionData;

    return *this;
}

Client::~Client()
{
    // std::cout << "Client:: Destructor Called (name: " << index << ")" << std::endl;
}


// =================== GETTERS ===============================

int Client::getFd() const
{
    return _fd;
}

enum ConnState Client::getConnState() const
{
    return _connState;
}

time_t Client::getStartTime() const
{
    return _startTime;
}

enum connectionType Client::getConnType() const
{
    return _connType;
}

const std::string& Client::getLocationPath() const
{
    return _locationPath;
}

// =================== SETTERS ===============================

void Client::setFd(int fd)
{
    _fd = fd;
}

void Client::setConnState(enum ConnState connState)
{
    _connState = connState;
}

void Client::setConnType(enum connectionType connType)
{
    _connType = connType;
}

void Client::setLocationPath(std::string locationPath)
{
    _locationPath = locationPath;
}

// =================== BUFFER METHODS ===============================

const std::string& Client::getBuffer() const
{
    return _buffer;
}

void Client::clearBuffer()
{
    _buffer.clear();
}

void Client::assignServerByServerName(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
                                            std::pair<std::string, std::string> ipPort, Server& defaultServer)
{
    const std::string& hostStr = request.getHeader("Host");

    size_t colonPos = hostStr.find(":");
    std::string serverName = request.getHeader("Host").substr(0, colonPos);

    std::map<std::pair<std::string, std::string>, std::vector<Server> >::iterator it = servers.begin();
    for (; it != servers.end(); ++it)
    {
        if (it->first == ipPort) {
            std::vector<Server>& vec = it->second;
            std::vector<Server>::iterator serverIt = vec.begin();
            for (; serverIt != vec.end(); ++serverIt) {
                if (serverIt->getServerName() == serverName) {
                    this->server = *serverIt;
                    return ;
                }
            }
        }
    }

    // if ServerName not match -> pick defaultServer
    this->server = defaultServer;
}

const std::string& Client::getSessionId() const
{
	return _sessionId;
}

const std::string& Client::getSessionData() const
{
	return _sessionData;
}

void Client::setSessionId(const std::string& sessionId)
{
	_sessionId = sessionId;
}

void Client::setSessionData(const std::string& sessionData)
{
	_sessionData = sessionData;
}

std::ostream & operator<<( std::ostream & o, Client const & client )
{
    o << "\nClient: \n"
        << "fd    : " << client.getFd() << '\n'
        << "buffer: "<< client.getBuffer() << '\n';
    return o;
}
