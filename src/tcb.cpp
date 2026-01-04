#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../test/printing.hpp"

TCB* TCB::running = nullptr;
long TCB::gPid = 0;
long TCB::tcbCount = 0;
/*int TCB::maxUserThreads = 5;
int TCB::currentUserThreads = -2;
List<TCB> TCB::waitingThreads;

void TCB::SetMaximumThreads(int num_of_threads) {
    maxUserThreads = num_of_threads;
}*/

TCB *TCB::createThread(TCB::Body body, void* arg) {
    TCB* thr = new TCB(body, arg);
    if(!thr->isMain()) {
        /*if (currentUserThreads >= maxUserThreads) {
            waitingThreads.addFirst(thr);
        }
        else {
            Scheduler::put(thr);
        }
        currentUserThreads++;*/
        Scheduler::put(thr);
    }
    return thr;
}

void TCB::yield() {
    uint64 op = 0x13;
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (op));
    __asm__ volatile ("ecall");
}

void TCB::dispatch() {
    TCB *old = running;

    /* if(old->isPinged()){
	    old->dumpMemoryInfo();
	    old->setPingFlag(false);
        }*/

    if (!old->isFinished() && !old->isBlocked()) { Scheduler::put(old); }
    running = Scheduler::get();
    if (running){
        running->isMain() ? Riscv::ms_sstatus(Riscv::SSTATUS_SPP) : Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
        TCB::contextSwitch(&old->context, &running->context);
    }
}

int TCB::exit() {
    if(running->isFinished()) return -1;
    running->setFinished(true);
    dispatch();
    return 0;
}

void TCB::threadWrapper() {
    Riscv::popSppSpie();
    running->isMain() ? running->cppThread->run() : running->body(running->args);
    running->setFinished(true);
    /*currentUserThreads--;
    if (!waitingThreads.isEmpty()) {
        Scheduler::put(waitingThreads.removeLast());
    }*/
    tcbCount--;
    TCB::yield();
}

int TCB::getId() {
    long pid = running ? running->getPid() : -1;
    dispatch();
    return (int)pid;
}

void TCB::join(TCB* handle) {
    while (!handle->isFinished()) {
        dispatch();
    }
}

/*void TCB::pair(TCB* t1, TCB* t2) {
    t1->setPartner(t2);
    t2->setPartner(t1);
}

void TCB::sync() {
    TCB* curr = running;
    TCB* part = curr->getPartner();

    if (!part) {
        return;
    }

    if (part->isWaitingForSync()) {
        curr->setWaitingForSync(false);
        part->setWaitingForSync(false);
        part->unblock();
        Scheduler::put(part);
    }else {
        curr->setWaitingForSync(true);
        curr->block();
        dispatch();
    }
}

void TCB::send(TCB* handle, const char* message) {
    //TCB* current = running;

    // Čekaj dok primaoc ne primi prethodnu poruku
    while (handle->hasMessage()) {
        Thread::dispatch();  // Busy wait
    }

    // Pošalji poruku
    handle->setMessage(message);

}

char* TCB::receive() {
    TCB* current = running;

    // Čekaj dok ne stigne poruka
    while (!current->hasMessage()) {
        Thread::dispatch();
    }

    // Vrati poruku i oslobodi memoriju
    char* msg = current->getMessage();
    current->clearMessage();
    return msg;
}

void TCB::setMessage(const char* msg) {
    // Alociraj memoriju za poruku
    if (message) {
        delete[] message;  // Oslobodi staru poruku ako postoji
    }

    size_t len = 0;
    while (msg[len] != '\0') len++;

    message = new char[len + 1];
    for (size_t i = 0; i <= len; i++) {
        message[i] = msg[i];
    }
}

void TCB::clearMessage() {
    if (message) {
        delete[] message;
        message = nullptr;
    }
}

void TCB::ping(TCB* handle) {
    handle->setPinged(true);
}

void TCB::addAllocation(void* address, size_t size) {
    if(address==nullptr) return;

    Allocation* newBlock = new Allocation(address, size);
    newBlock->next = allocatedMemory;
    allocatedMemory = newBlock;
}

void TCB::removeAllocation(void* address) {
    if(address==nullptr || allocatedMemory == nullptr) return;
    
    Allocation* curr = allocatedMemory;
    Allocation* prev = nullptr;

    while(curr != nullptr) {
        if(curr->address = address) {
             if(prev == nullptr) {
                   allocatedMemory = current->next;
	     } else{ prev->mext = current->next;}
             delete curr;
             return;
	}
        prev = curr;
        curr = curr->next;
    }
}

void TCB::dumpMemoryInfo() {
    if (allocatedMemory == nullptr) {
        // Ispis: "Thread [ID]: No allocated memory"
        printString("Thread ");
        printInt(threadId);
        printString(": No allocated memory\n");
        return;
    }
    
    printString("Thread ");
    printInt(threadId);
    printString(" allocated memory:\n");
    
    Allocation* current = allocatedMemory;
    int blockCount = 0;
    
    while (current != nullptr) {
        // Ispis: "  Block X: Address=0x..., Size=Y blocks"
        printString("  Block ");
        printInt(++blockCount);
        printString(": Address=0x");
        printHex((uint64)current->address);
        printString(", Size=");
        printInt(current->size / MEM_BLOCK_SIZE);
        printString(" blocks\n");
        
        current = current->next;
    }
}

*/







