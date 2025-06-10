#include "../../include/Connection.h"
#include <iostream>

Connection::Connection(int index, int fd)
	: index(index), fd(fd), buffer("")
{
	// std::cout << "Connection:: Constructor Called (name: " << index << ")" << std::endl;


}

Connection::Connection(const Connection& other)
	: index(other.index), fd(other.fd), buffer(other.buffer)
{
	// std::cout << "Connection:: Copy Constructor Called" << std::endl;
}

Connection& Connection::operator=(const Connection& other)
{
	// std::cout << "Connection:: Copy Assignment Operator Called" << std::endl;
	
	if (this == &other)
		return *this;
	buffer = other.buffer;

	return *this;
}

Connection::~Connection()
{
	// std::cout << "Connection:: Destructor Called (name: " << index << ")" << std::endl;
}
	

void Connection::appendToBuffer(std::string str)
{
	buffer += str;
}

const std::string& Connection::getBuffer() const
{
	return buffer;
}


std::ostream & operator<<( std::ostream & o, Connection const & connection )
{
	o << "\nConnection: \n" 
		<< "index : " << connection.index << '\n'
		<< "fd    : " << connection.fd << '\n'
		<< "buffer: "<< connection.getBuffer() << '\n';
	return o;
}


