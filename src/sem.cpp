#include "../h/sem.hpp"
#include "../h/tcb.hpp"

Sem* Sem::open(unsigned int i) { return new Sem(i); }

int Sem::signal() {
    if(isOpen()) {
        if (!blockedList.isEmpty()) {
            TCB* t = blockedList.removeFirst();
            t->unblock();
            Scheduler::put(t);
        } else {
            ++value;
        }
        return 0;
    }
    return -1;
}

int Sem::wait() {
    if(isOpen()) {
        if (value <= 0) {
            TCB::running->block();
            blockedList.addLast(TCB::running);
            TCB::dispatch();

            if (!isOpen()) {
                return -1;
            }
        } else {
            --value;
        }
        return 0;
    }
    return -1;
}

int Sem::close(){
    if(isOpen()) {
        while (!blockedList.isEmpty()) {
            TCB* t = blockedList.removeFirst();
            //t->setFinished(false);
            Scheduler::put(t);
        }
        setOpen(false);
        return 0;
    }
    return -1;
}

Sem::~Sem(){
    close();
}