
#ifndef _FIXEDSIZECHUNKALLOCATOR_H_
#define _FIXEDSIZECHUNKALLOCATOR_H_

#include "common.h"
#include "logger/Logger.h"
#include <stdint.h>

namespace miniSQL {

class FixedSizeChunkAllocator
{
public:
    FixedSizeChunkAllocator();
    ~FixedSizeChunkAllocator();

    DISALLOW_COPY_AND_ASSIGN(FixedSizeChunkAllocator);
    
public:
    bool init(uint32_t requestSize, uint32_t maxRequestChunk);
    void release();
    void* allocate(uint32_t size);
    void free(void* const addr);

private:
    uint32_t _fixedSize;
    char *_free;
    char *_end;
    void *_freeList;
    uint32_t _count;
    uint32_t _maxRequestChunk;
    char *_buffer;

private:
    DECLARE_LOGGER(FixedSizeChunkAllocator);
};

}

#endif
