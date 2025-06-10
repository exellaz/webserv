
#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>

class Connection {
public:
	// Constructor
	Connection(int index, int fd);
	// Copy Constructor
	Connection(const Connection& other);
	// Copy Assignment Operator
	Connection& operator=(const Connection& other);
	// Destructor
	~Connection();

	const int index;
	const int fd;
	
	void appendToBuffer(char *str);
	const std::string getBuffer() const;
	void setBuffer(std::string str);
	void clearBuffer();

private:
	std::string buffer;
};

std::ostream & operator<<( std::ostream & o, Connection const & connection );

#endif
