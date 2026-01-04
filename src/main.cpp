#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/sem.hpp"


extern void userMain();

void userMainWrapper(void*){
    userMain();
}

void idle(void*) {
    while(true) {
        thread_dispatch();
    }
}


int main(){

    TCB *threads[5];

    __asm__ volatile ("csrw stvec, %0"::"r"(&Riscv::supervisorTrap));
    //__asm__ volatile ("csrs sstatus, 0x2");
    userModeCall();
    thread_create(&threads[0], nullptr, nullptr);
    TCB::running = threads[0];

    thread_create(&threads[2], idle, nullptr);
    thread_create(&threads[1], &userMainWrapper, nullptr);

    while(!threads[1]->isFinished()) {
        thread_dispatch();
    }

    return 0;
}