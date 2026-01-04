#include "../h/riscv.hpp"
#include "../lib/console.h"
#include "../h/tcb.hpp"
#include "../h/sem.hpp"
#include "../test/printing.hpp"

void Riscv::popSppSpie() {
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

using Body = void (*)(void *);

void Riscv::handleSupervisorTrap() {
    uint64 code;
    __asm__ volatile ("mv %[x], a0" : [x] "=r" (code));
    uint64 a1arg;
    __asm__ volatile ("mv %[x], a1" : [x] "=r" (a1arg));
    uint64 a2arg;
    __asm__ volatile ("mv %[x], a2" : [x] "=r" (a2arg));
    uint64 a3arg;
    __asm__ volatile ("mv %[x], a6" : [x] "=r" (a3arg));
    uint64 scause = r_scause();
    if(scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        uint64 sepc = r_sepc() + 4;
        uint64 sstatus = r_sstatus();
        switch (code) {
            case 0x01: {// mem_alloc:
                void* res = MemoryAllocator::mem_alloc(a1arg);
                __asm__ volatile ("mv a0, %0" : : "r" (res));
                __asm__ volatile ("sd a0, 80(s0)");
                break;
            }
            case 0x02: {// mem_free
                void* addr = (void*) a1arg;
                int res = MemoryAllocator::mem_free(addr);
                __asm__ volatile ("mv a0, %0" : : "r" (res));
                __asm__ volatile ("sd a0, 80(s0)");
                break;
            }
            case 0x03: {// get_free_space:
                uint64 res = MemoryAllocator::get_free_space();
                __asm__ volatile ("mv a0, %0" : : "r" (res));
                __asm__ volatile ("sd a0, 80(s0)");
                break;
            }
            case 0x04: {// get_largest_free_block:
                uint64 res = MemoryAllocator::get_largest_free_block();
                __asm__ volatile ("mv a0, %0" : : "r" (res));
                __asm__ volatile ("sd a0, 80(s0)");
                break;
            }
            case 0x11: {// thread_create
                auto handle = (TCB**) a1arg;
                void (*start_routine)(void*);
                start_routine=reinterpret_cast<void(*)(void*)>(a2arg);
                auto arg=(void* )a3arg;
                *handle = TCB::createThread(start_routine, arg);
                int res = (*handle) ? 0 : -1;

                __asm__ volatile ("mv a0, %0" : : "r" (res));
                __asm__ volatile ("sd a0, 80(s0)");
                break;
            }
            case 0x12: {// thread_exit
                uint64 res = TCB::exit();
                __asm__ volatile ("mv a0, %0" : : "r" (res));
                __asm__ volatile ("sd a0, 80(s0)");
                break;
            }
            case 0x13: {// thread_dispatch
                TCB::dispatch();
                break;
            }
            case 0x14: {
                int res = TCB::getId();
                __asm__ volatile ("mv a0, %0" : : "r" (res));
                __asm__ volatile ("sd a0, 80(s0)");
                break;
            }
            /*case 0x15: {
                TCB* t1 = (TCB*)a1arg;
                TCB* t2 = (TCB*)a2arg;
                TCB::pair(t1, t2);
                break;
            }
            case 0x16:  {
                TCB::running->sync();
                break;
            }
            case 0x17: {
                int max = (int)a1arg;
                TCB::SetMaximumThreads(max);
                break;
            }*/
            case 0x18: { // thread_join
                TCB* target = (TCB*)a1arg;
                TCB::join(target);
                break;
            }
            /*case 0x19: { // thread_send
                TCB* target = (TCB*)a1arg;
                char* message = (char*)a2arg;
                TCB::send(target, message);
                break;
            }
            case 0x1A: { // thread_receive
                char* message = TCB::receive();
                __asm__ volatile ("mv a0, %0" : : "r" (message));
                __asm__ volatile ("sd a0, 80(s0)");
                break;
            }*/
            case 0x21: {// sem_open
                auto init = (unsigned int) a2arg;
                auto handle = (sem_t*) a1arg;
                *handle = Sem::open(init);
                uint64 res = (*handle) ? 0 : -1;
                __asm__ volatile ("mv a0, %0" : : "r" (res));
                __asm__ volatile ("sd a0, 80(s0)");
                break;
            }
            case 0x22: {// sem_close
                auto handle = (sem_t) a1arg;
                uint64 res = handle ? handle->close() : -1;
                __asm__ volatile ("mv a0, %0" : : "r" (res));
                __asm__ volatile ("sd a0, 80(s0)");
                break;
            }
            case 0x23: {// sem_wait
                auto handle = (sem_t) a1arg;
                uint64 res = handle ? handle->wait() : -1;
                __asm__ volatile ("mv a0, %0" : : "r" (res));
                __asm__ volatile ("sd a0, 80(s0)");
                break;
            }
            case 0x24: {// sem_signal
                auto handle = (sem_t) a1arg;
                uint64 res = handle ? handle->signal() : -1;
                __asm__ volatile ("mv a0, %0" : : "r" (res));
                __asm__ volatile ("sd a0, 80(s0)");
                break;
            }
            case 0x41: {// get_c
                char c = __getc();
                __asm__ volatile ("mv a0, %0" : : "r" (c));
                __asm__ volatile ("sd a0, 80(s0)");
                break;
            }
            case 0x42: {// put_c
                auto c = (char) a1arg;
                __putc(c);
                break;
            }
            case 0x50: {
                sstatus &= ~SSTATUS_SPP;
                break;
            }
            default: break;
        }
        w_sstatus(sstatus);
        w_sepc(sepc);

    } else if (scause == 0x8000000000000001UL) {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        uint64 sepc = r_sepc();
        uint64 sstatus = r_sstatus();
        mc_sip(SIP_SSIP);
        w_sstatus(sstatus);
        w_sepc(sepc);
    } else if (scause == 0x8000000000000009UL) {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        console_handler();
    } else {
        // unexpected trap cause
        /*printString("Greska: ");
        printInt(scause);
        printString("! \n");*/
    }
}
