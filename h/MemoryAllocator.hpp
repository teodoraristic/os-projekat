#ifndef _MEMORY_ALLOCATOR_HPP
#define _MEMORY_ALLOCATOR_HPP

#include "../lib/hw.h"

struct FreeMem {
    FreeMem* next;
    FreeMem* prev;
    size_t size;
};

struct TakenMem {
    TakenMem* next;
    TakenMem* prev;
    size_t size;
};

/*class TCB;*/

class MemoryAllocator {
private:

    static FreeMem* freeMemHead;
    static TakenMem* takenMemHead;
    static MemoryAllocator* instance;
    static int join(FreeMem*);
    static bool initialized;

public:
    static MemoryAllocator* getInstance();
    static void init();

    static void* mem_alloc(size_t size);
    static int mem_free(void* ptr);
    static uint64  get_largest_free_block();
    static uint64  get_free_space();

};

#endif