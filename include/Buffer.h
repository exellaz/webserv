#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <unistd.h>

struct Buffer {
    std::vector<char> data;
    size_t lastIndex;

    Buffer(size_t sz)
        : data(sz), lastIndex(0) {}

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
