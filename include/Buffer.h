#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <unistd.h>
#include <string>

struct Buffer {
    std::vector<char> data;
    size_t lastIndex;
	
	Buffer() {}
	 
    Buffer(size_t sz)
        : data(sz), lastIndex(0) {}
	
	Buffer(const std::string& str) 
		: data(str.begin(), str.end()), lastIndex(str.length()) 
	{
		data.push_back('\0');
	}

    size_t remainingSize() const {
        return data.size() - lastIndex;
    }

    char* last() {
        return &data[lastIndex];
    }

    void advance(size_t n) {
        lastIndex += n;
    }

    size_t dataSize() const {
        return lastIndex;
    }
};


#endif
