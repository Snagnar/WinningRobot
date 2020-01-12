#ifndef RINGBUFFER
#define RINGBUFFER

#include <stdio.h>
#include <Arduino.h>

class RingBuffer {
    RingBuffer(int, int);
    int write(char*);
    int read(int);
    int readAll(char, char*);

    private:
    size_t _size;
    char _buffer[][];
    int _pos;
};

#endif //RINGBUFFER