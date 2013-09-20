
#ifndef FIXEDSIZECHUNKALLOCATOR_HPP
#define FIXEDSIZECHUNKALLOCATOR_HPP

#include "common.h"

class FixedSizeChunkAllocator
{
public:
    FixedSizeChunkAllocator();
    ~FixedSizeChunkAllocator();

    DISALLOW_COPY_AND_ASSGIN(FixedSizeChunkAllocator);
    
public:
    void init(uint32_t requestSize, uint32_t maxRequestChunk)
    void release();
    void* allocate(uint32_t size);
    void free(void* const addr);

private:
    size_t _fixedSize;
    char *_free;
    char *_end;
    void *_freeList;
    size_t _count;
    size_t _maxRequestChunk;
    char *_buffer;
};

#endif
