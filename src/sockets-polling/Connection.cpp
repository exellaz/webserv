#include "../../include/Connection.h"

Connection::Connection(int fd, time_t startTime) :
	fd(fd), connState(ACTIVE), startTime(startTime), connType(KEEP_ALIVE),
	readBodyMethod(CONTENT_LENGTH), contentLength(0),
	readChunkedRequestStatus(READ_CHUNK_SIZE), chunkSize(0),
	chunkReqBuf(""), isFirstTimeReadingBody(true), isResponseReady(false),
	_buffer("")
{
	// std::cout << "Connection:: Constructor Called (name: " << index << ")" << std::endl;


}

Connection::Connection(const Connection& other) :
	fd(other.fd), connState(other.connState),startTime(other.startTime), connType(other.connType),
	readBodyMethod(other.readBodyMethod), contentLength(other.contentLength),
	readChunkedRequestStatus(other.readChunkedRequestStatus), chunkSize(other.chunkSize),
	chunkReqBuf(other.chunkReqBuf), isFirstTimeReadingBody(other.isFirstTimeReadingBody),
	isResponseReady(other.isResponseReady), _buffer(other._buffer)
{
	// std::cout << "Connection:: Copy Constructor Called" << std::endl;
}

Connection& Connection::operator=(const Connection& other)
{
	// std::cout << "Connection:: Copy Assignment Operator Called" << std::endl;

	if (this == &other)
		return *this;

	connState 				 = other.connState;
	startTime 				 = other.startTime;
	connType 				 = other.connType;
	readBodyMethod 			 = other.readBodyMethod;
	contentLength 			 = other.contentLength;
	readChunkedRequestStatus = other.readChunkedRequestStatus;
	chunkSize 				 = other.chunkSize;
	chunkReqBuf 			 = other.chunkReqBuf;
	isFirstTimeReadingBody 	 = other.isFirstTimeReadingBody;
	isResponseReady 		 = other.isResponseReady;
	_buffer 				 = other._buffer;

	return *this;
}

Connection::~Connection()
{
	// std::cout << "Connection:: Destructor Called (name: " << index << ")" << std::endl;
}


void Connection::appendToBuffer(const char *str, size_t n)
{
	_buffer.append(str, n);
}

const std::string& Connection::getBuffer() const
{
	return _buffer;
}

void Connection::setBuffer(std::string str)
{
	_buffer = str;
}

void Connection::clearBuffer()
{
	_buffer.clear();
}

void Connection::eraseBufferFromStart(size_t n)
{
	_buffer.erase(0, n);
}

size_t Connection::bufferSize() const
{
	return _buffer.size();
}

bool Connection::compareBuffer(const std::string str)
{
	if (_buffer == str)
		return true;
	return false;
}

size_t Connection::findInBuffer(const std::string str, size_t pos)
{
	return _buffer.find(str, pos);
}

void Connection::assignServerByServerName(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
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

std::ostream & operator<<( std::ostream & o, Connection const & connection )
{
	o << "\nConnection: \n"
		<< "fd    : " << connection.fd << '\n'
		<< "buffer: "<< connection.getBuffer() << '\n';
	return o;
}
