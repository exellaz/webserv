#include "../../include/Connection.h"

Connection::Connection(int fd, time_t startTime) :
	fd(fd), startTime(startTime), connType(KEEP_ALIVE),
	readBodyMethod(CONTENT_LENGTH), contentLength(0),
	isResponseReady(false), _buffer("")
{
	// std::cout << "Connection:: Constructor Called (name: " << index << ")" << std::endl;


}

Connection::Connection(const Connection& other) :
	fd(other.fd), startTime(other.startTime), connType(other.connType),
	readBodyMethod(other.readBodyMethod), contentLength(other.contentLength),
	isResponseReady(other.isResponseReady), _buffer(other._buffer)
{
	// std::cout << "Connection:: Copy Constructor Called" << std::endl;
}

Connection& Connection::operator=(const Connection& other)
{
	// std::cout << "Connection:: Copy Assignment Operator Called" << std::endl;

	if (this == &other)
		return *this;

	fd 				= other.fd;
	startTime 		= other.startTime;
	connType 		= other.connType;
	readBodyMethod 	= other.readBodyMethod;
	contentLength 	= other.contentLength;
	isResponseReady = other.isResponseReady;
	_buffer 		= other._buffer;

	return *this;
}

Connection::~Connection()
{
	// std::cout << "Connection:: Destructor Called (name: " << index << ")" << std::endl;
}


void Connection::appendToBuffer(char* str, size_t n)
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

std::ostream & operator<<( std::ostream & o, Connection const & connection )
{
	o << "\nConnection: \n"
		<< "fd    : " << connection.fd << '\n'
		<< "buffer: "<< connection.getBuffer() << '\n';
	return o;
}


