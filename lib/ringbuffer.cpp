#include "ringbuffer.h"

RingBuffer::RingBuffer(int length, int string_length) {
    _size = length;
    _buffer = malloc(length * sizeof(char*));
    for(int x = 0; x<length; x++)
        _buffer[x] = malloc(string_length * sizeof(char));
    _pos = 0;
}

int RingBuffer::write(char* content) {
    strcopy(_buffer[_pos++], content);
    _pos %=_size;
}

int read(int offset) {
    return _buffer[(_pos - offset) % _size];
}