#include "Client.h"

Client::Client(int fd, time_t startTime) :
	fd(fd), connState(ACTIVE), startTime(startTime), connType(KEEP_ALIVE),
	readBodyMethod(CONTENT_LENGTH), contentLength(0),
	readChunkedRequestStatus(READ_CHUNK_SIZE), chunkSize(0),
	chunkReqBuf(""), isFirstTimeReadingBody(true), isResponseReady(false),
	_buffer("")
{
	// std::cout << "Client:: Constructor Called (name: " << index << ")" << std::endl;


}

Client::Client(const Client& other) :
	fd(other.fd), connState(other.connState),startTime(other.startTime), connType(other.connType),
	readBodyMethod(other.readBodyMethod), contentLength(other.contentLength),
	readChunkedRequestStatus(other.readChunkedRequestStatus), chunkSize(other.chunkSize),
	chunkReqBuf(other.chunkReqBuf), isFirstTimeReadingBody(other.isFirstTimeReadingBody),
	isResponseReady(other.isResponseReady), _buffer(other._buffer)
{
	// std::cout << "Client:: Copy Constructor Called" << std::endl;
}

Client& Client::operator=(const Client& other)
{
	// std::cout << "Client:: Copy Assignment Operator Called" << std::endl;

	if (this == &other)
		return *this;

	fd						 = other.fd;
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

Client::~Client()
{
	// std::cout << "Client:: Destructor Called (name: " << index << ")" << std::endl;
}


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

std::ostream & operator<<( std::ostream & o, Client const & client )
{
	o << "\nClient: \n"
		<< "fd    : " << client.fd << '\n'
		<< "buffer: "<< client.getBuffer() << '\n';
	return o;
}
