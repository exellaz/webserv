#include "../../include/Client.h"

Client::Client(int fd, time_t startTime) :
    _fd(fd), _connState(ACTIVE), _startTime(startTime), _connType(KEEP_ALIVE),
    _buffer(""), _readBodyMethod(CONTENT_LENGTH), _contentLength(0),
    _readChunkedRequestStatus(READ_CHUNK_SIZE), _chunkSize(0),
    _chunkReqBuf(""), _locationPath(""),
    _firstTimeReadingBody(true), _responseReady(false)
    
{
    // std::cout << "Client:: Constructor Called (name: " << index << ")" << std::endl;


}

Client::Client(const Client& other) :
    _fd(other._fd), _connState(other._connState), _startTime(other._startTime), _connType(other._connType),
    _buffer(other._buffer), _readBodyMethod(other._readBodyMethod), _contentLength(other._contentLength),
    _readChunkedRequestStatus(other._readChunkedRequestStatus), _chunkSize(other._chunkSize),
    _chunkReqBuf(other._chunkReqBuf), _locationPath(other._locationPath),
    _firstTimeReadingBody(other._firstTimeReadingBody), _responseReady(other._responseReady)
{
    // std::cout << "Client:: Copy Constructor Called" << std::endl;
}

Client& Client::operator=(const Client& other)
{
    // std::cout << "Client:: Copy Assignment Operator Called" << std::endl;

    if (this == &other)
        return *this;

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
    _locationPath			 = other._locationPath;
    _firstTimeReadingBody 	 = other._firstTimeReadingBody;
    _responseReady 		 = other._responseReady;

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

enum readBodyMethod Client::getReadBodyMethod() const
{
    return _readBodyMethod;
}

size_t Client::getContentLength() const
{
    return _contentLength;
}

enum readChunkedRequestStatus Client::getReadChunkedRequestStatus() const
{
    return _readChunkedRequestStatus;
}

size_t Client::getChunkSize() const
{
    return _chunkSize;
}

const std::string& Client::getLocationPath() const
{
    return _locationPath;
}

bool Client:: isFirstTimeReadingBody() const
{
    return _firstTimeReadingBody;
}
bool Client::isResponseReady() const
{
    return _responseReady;
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

void Client::setStartTime(time_t startTime)
{
    _startTime = startTime;
}

void Client::setConnType(enum connectionType connType)
{
    _connType = connType;
}

void Client::setReadBodyMethod(enum readBodyMethod readBodyMethod)
{
    _readBodyMethod = readBodyMethod;
}

void Client::setContentLength(size_t contentLength)
{
    _contentLength = contentLength;
}

void Client::setReadChunkedRequestStatus(enum readChunkedRequestStatus chunkedRequestStatus)
{
    _readChunkedRequestStatus = chunkedRequestStatus;
}

void Client::setChunkSize(size_t chunkSize)
{
    _chunkSize = chunkSize;
}

void Client::setLocationPath(std::string locationPath)
{
    _locationPath = locationPath;
}

void Client::setFirstTimeReadingBody(bool status)
{
    _firstTimeReadingBody = status;
}
void Client::setResponseReady(bool status)
{
    _responseReady = status;
}


// =================== BUFFER METHODS ===============================

void Client::appendToBuffer(const char *str, size_t n)
{
    _buffer.append(str, n);
}

const std::string& Client::getBuffer() const
{
    return _buffer;
}

void Client::setBuffer(std::string str)
{
    _buffer = str;
}

void Client::clearBuffer()
{
    _buffer.clear();
}

void Client::eraseBufferFromStart(size_t n)
{
    _buffer.erase(0, n);
}

size_t Client::bufferSize() const
{
    return _buffer.size();
}

bool Client::compareBuffer(const std::string str)
{
    if (_buffer == str)
        return true;
    return false;
}

size_t Client::findInBuffer(const std::string str, size_t pos)
{
    return _buffer.find(str, pos);
}

// =================== chunkedReqBuf METHODS ===============================

void Client::appendToChunkReqBuf(const char *str, size_t n)
{
    _chunkReqBuf.append(str, n);
}

const std::string& Client::getChunkReqBuf() const
{
    return _chunkReqBuf;
}

void Client::setChunkReqBuf(std::string str)
{
    _chunkReqBuf = str;
}

void Client::clearChunkReqBuf()
{
    _chunkReqBuf.clear();
}

void Client::eraseChunkReqBufFromStart(size_t n)
{
    _chunkReqBuf.erase(0, n);
}

size_t Client::chunkReqBufSize() const
{
    return _chunkReqBuf.size();
}

bool Client::compareChunkReqBuf(const std::string str)
{
    if (_chunkReqBuf == str)
        return true;
    return false;
}

size_t Client::findInChunkReqBuf(const std::string str, size_t pos)
{
    return _chunkReqBuf.find(str, pos);
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