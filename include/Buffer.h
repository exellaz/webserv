#ifndef BUFFER_H
#define BUFFER_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

#define NGX_AGAIN -1
#define NGX_OK 0
#define NGX_ERROR -2
#define NGX_REQUEST_HEADER_TOO_LARGE -431
#define CLIENT_HEADER_BUFFER_SIZE 4096
#define LARGE_HEADER_BUFFER_SIZE 8192
#define MAX_LARGE_BUFFERS 4

struct Buffer {
    char* start;
    char* end;
    char* last;
    size_t size;

    Buffer(size_t sz)
        : size(sz)
    {
        start = new char[size];
        end = start + size;
        last = start;
    }

    Buffer(const Buffer& other)
        : size(other.size)
    {
        start = new char[size];
        end = start + size;
        last = start + (other.last - other.start);
        std::memcpy(start, other.start, other.last - other.start);
    }

    Buffer& operator=(const Buffer& other) {
        if (this != &other) {
            delete[] start;
            size = other.size;
            start = new char[size];
            end = start + size;
            last = start + (other.last - other.start);
            std::memcpy(start, other.start, other.last - other.start);
        }
        return *this;
    }

    ~Buffer() {
        delete[] start;
    }

    size_t remainingSize() const {
        return end - last;
    }
};

#endif
