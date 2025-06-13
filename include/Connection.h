
#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>

enum readBodyMethod {
	CONTENT_LENGTH,
	CHUNKED_ENCODING,
	NO_BODY,
};

enum connectionType {
	CLOSE,
	KEEP_ALIVE,
};

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
	enum connectionType connType;
	enum readBodyMethod readBodyMethod;
	size_t contentLength;
	bool isResponseReady;
	
	void appendToBuffer(char *str);
	const std::string& getBuffer() const;
	void setBuffer(std::string str);
	void clearBuffer();

private:
	std::string _buffer;
};

std::ostream & operator<<( std::ostream & o, Connection const & connection );

#endif
