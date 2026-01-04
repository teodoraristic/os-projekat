#include "../h/scheduler.hpp"
#include "../h/tcb.hpp"

List<TCB> Scheduler::threadQueue;


void Scheduler::put(TCB* tcb) {
    threadQueue.addLast( tcb );
}

TCB* Scheduler::get() {
    return threadQueue.removeFirst();
}