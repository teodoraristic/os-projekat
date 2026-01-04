#include "../h/MemoryAllocator.hpp"

FreeMem* MemoryAllocator::freeMemHead = nullptr;
TakenMem* MemoryAllocator::takenMemHead = nullptr;
MemoryAllocator* MemoryAllocator::instance = nullptr;
bool MemoryAllocator::initialized = false;


void MemoryAllocator::init() {
    if (!initialized) {
        initialized=true;
        freeMemHead = (FreeMem*) HEAP_START_ADDR;
        freeMemHead->size = (size_t) HEAP_END_ADDR - (size_t) HEAP_START_ADDR;
        freeMemHead->next = freeMemHead->prev = nullptr;
        getInstance();
    }
}

MemoryAllocator* MemoryAllocator::getInstance() {
    if (!initialized) { init(); }
    return instance;
}


void* MemoryAllocator::mem_alloc(size_t size) {
    if (size == 0) {
        return nullptr;
    }
    if (!initialized) {
        init();
    }

    size_t totalSize = (size+MEM_BLOCK_SIZE-1+sizeof(TakenMem))/MEM_BLOCK_SIZE;

    totalSize*=MEM_BLOCK_SIZE;

    for (FreeMem* cur=freeMemHead; cur; cur=cur->next) {
        if (cur->size<totalSize) continue;
        if (cur->size - totalSize < sizeof(FreeMem)) {
            cur->prev ? cur->prev->next = cur->next : freeMemHead = cur->next;
            if (cur->next) cur->next->prev = cur->prev;
        }
        else {
            auto newFM = (FreeMem*)((char*)cur+totalSize);
            newFM->prev = cur->prev;
            newFM->next = cur->next;
            newFM->size = cur->size - totalSize;
            cur->prev ? cur->prev->next = newFM : freeMemHead = newFM;
            if (cur->next) cur->next->prev = newFM;
        }

        TakenMem* t = nullptr;
        if(takenMemHead && (char*)cur > (char*)takenMemHead)
            for(t=takenMemHead; t->next && (char*)cur > (char*) t->next; t=t->next);
        auto newSeg = (TakenMem*) cur;
        newSeg->prev = t;
        newSeg->size = totalSize;
        newSeg->next = t ? t->next : takenMemHead;
        if(newSeg->next) newSeg->next->prev = newSeg;
        t ? t->next = newSeg : takenMemHead = newSeg;


        /*void* result = (char*)cur + sizeof(TakenMem);
        
        if (TCB::running != nullptr) {
            TCB::running->addAllocatedBlock(result, totalSize);
        }*/

        return (char*)cur + sizeof(TakenMem);
    }

    return nullptr;


}

int MemoryAllocator::mem_free(void* ptr) {
    if (!ptr) return -1;
    if (!initialized) return -4;


    /*if (TCB::running != nullptr) {
        TCB::running->removeAllocatedBlock(ptr);
    }*/

    TakenMem* t;

    auto tm = (TakenMem*)((char*)ptr - sizeof(TakenMem));
    for (t = takenMemHead; t && t<=tm; t = t->next) {
        if (t==tm){
            goto delete_taken;
        }
    }
    return -2;

    delete_taken:
    t->prev ? t->prev->next = t->next : takenMemHead = t->next;
    if (t->next) t->next->prev = t->prev;

    FreeMem* cur;
    if (!freeMemHead || ptr<(char*)freeMemHead) {
        cur=nullptr;
    }
    else{
        for(cur=freeMemHead; cur->next!=nullptr && ptr>(char*)(cur->next); cur=cur->next);
    }
    auto newSeg = (FreeMem*)tm;

    newSeg->prev = cur;
    if (cur) newSeg->next=cur->next;
    else newSeg->next=freeMemHead;
    if (newSeg->next) newSeg->next->prev = newSeg;
    if (cur) cur->next = newSeg;
    else freeMemHead=newSeg;

    join(newSeg);
    join(cur);
    return 0;
}

uint64 MemoryAllocator::get_free_space() {
    uint64 fspc = 0;
    if (!initialized) { init(); }
    for (auto curr = freeMemHead; curr != nullptr; curr = curr->next) {
        fspc += curr->size;
    }
    return fspc;
}

uint64 MemoryAllocator::get_largest_free_block() {
    uint64 larg_blk = 0;
    if (!initialized) { init(); }
    for (auto curr = freeMemHead; curr != nullptr; curr = curr->next) {
        if (curr->size > larg_blk) larg_blk = curr->size;
    }
    return larg_blk;
}

int MemoryAllocator::join(FreeMem * block) {
    if (!block) return 0;
    if (block->next && ((char*)block+block->size) == (char*)(block->next)){
        block->size += block->next->size;
        block->next = block->next->next;
        if (block->next) block->next->prev = block;
        return 1;
    }
    return 0;
}