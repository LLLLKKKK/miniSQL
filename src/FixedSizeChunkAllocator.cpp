
#include "FixedSizeChunkAllocator.hpp"
#include <cstdlib>
#include <cassert>

namespace miniSQL {

FixedSizeChunkAllocator::FixedSizeChunkAllocator() {
    _fixedSize = 0;
    _free = NULL;
    _end = NULL;
    _freeList = NULL;
    _count = 0;
    _maxRequestChunk = 0;
    _buffer = NULL;
}

FixedSizeChunkAllocator::~FixedSizeChunkAllocator() {
    release();
}

void FixedSizeChunkAllocator::init(uint32_t requestSize, uint32_t maxRequestChunk) {
    _maxRequestChunk = maxRequestChunk;
    _fixedSize = requestSize > sizeof(void *) ? requestSize : sizeof(void *);
    _buffer = (char *)(::malloc(requestSize * maxRequestChunk));
    _end = _buffer + requestSize * maxRequestChunk;
    _free = _buffer;
}

void* FixedSizeChunkAllocator::allocate(uint32_t size) {
    void *ret;
    ++_count;
    assert(_count <= _maxRequestChunk);
    if (_freeList) {
        ret = _freeList;
        _freeList  = *static_cast<void **> (_freeList);
    } else {
        assert(_free < _end);
        ret = _free;
        _free += _fixedSize;
    }
    return ret;
}

void FixedSizeChunkAllocator::free(void* const addr) {
    --_count;
    *reinterpret_cast<void **>(addr) = _freeList;
    _freeList = addr;
}

void FixedSizeChunkAllocator::release() {
    _free = NULL;
    _end = NULL;
    _freeList = NULL;
    if (_buffer) {
        ::free(_buffer);
        _buffer = NULL;
    }
}

}
