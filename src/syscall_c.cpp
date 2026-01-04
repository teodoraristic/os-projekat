#include "../h/syscall_c.hpp"
#include "../h/riscv.hpp"

void* mem_alloc(size_t sz){
    uint64 res;
    size_t size = sz;
    __asm__ volatile ("mv a1, %[s]" : : [s] "r" (size));
    __asm__ volatile ("mv a0, %[o]" : : [o] "r" (0x01));
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %[r], a0" : [r] "=r" (res));

    return (void*) res;
}

int mem_free(void* ptr){
    uint64 res;
    __asm__ volatile ("mv a1, %[x]" : : [x] "r" (ptr));
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x02));
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %[x], a0" : [x] "=r" (res));

    return res;
}

size_t mem_get_free_space(){
    uint64 res;
    __asm__ volatile ("mv a0, %[o]" : : [o] "r" (0x03));
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %[r], a0" : [r] "=r" (res));

    return res;
}

size_t mem_get_largest_free_block(){
    size_t res;
    __asm__ volatile ("mv a0, %[o]" : : [o] "r" (0x04));
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %[r], a0" : [r] "=r" (res));
    return res;
}

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg){
    uint64 volatile res;

    __asm__ volatile ("mv a6, %[x]" : : [x] "r" (arg));
    __asm__ volatile ("mv a2, %[x]" : : [x] "r" (start_routine));
    __asm__ volatile ("mv a1, %[x]" : : [x] "r" (handle));
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x11));
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %[x], a0" : [x] "=r" (res));

    return res;
}

int thread_exit() {
    uint64 res;
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x12));
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %[x], a0" : [x] "=r" (res));

    return res;
}

void thread_dispatch() {
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x13));
    __asm__ volatile ("ecall");
}

int getThreadId() {
    uint64 res;
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x14));
    __asm__ volatile ("ecall");
    __asm__ volatile ("mv %[x], a0" : [x] "=r" (res));
    return (int)res;
}

void thread_join(thread_t handle) {
    __asm__ volatile ("mv a1, %[x]" : : [x] "r" (handle));
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x18));
    __asm__ volatile ("ecall");
}

/*void thread_pair(thread_t t1, thread_t t2) {
    __asm__ volatile("mv a2, %[x]" : : [x] "r" (t2));
    __asm__ volatile("mv a1, %[y]" : : [y] "r" (t1));
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x15));
    __asm__ volatile("ecall");
}

void thread_sync() {
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x16));
    __asm__ volatile("ecall");
}

void set_max_threads(int max) {
    __asm__ volatile("mv a1, %[y]" : : [y] "r" (max));
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x17));
    __asm__ volatile("ecall");
}

void thread_send(thread_t handle, char* message) {
    __asm__ volatile ("mv a2, %[x]" : : [x] "r" (message));
    __asm__ volatile ("mv a1, %[x]" : : [x] "r" (handle));
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x19));
    __asm__ volatile ("ecall");
}

char* thread_receive() {
    uint64 res;
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x1A));
    __asm__ volatile ("ecall");
    __asm__ volatile ("mv %[x], a0" : [x] "=r" (res));
    return (char*)res;
}*/

int sem_open(sem_t* handle, unsigned init){
    uint64 res;
    __asm__ volatile ("mv a2, %[x]" : : [x] "r" (init));
    __asm__ volatile ("mv a1, %[x]" : : [x] "r" (handle));
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x21));
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %[x], a0" : [x] "=r" (res));

    return res;
}

int sem_close(sem_t handle){
    uint64 res;
    __asm__ volatile ("mv a1, %[x]" : : [x] "r" (handle));
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x22));
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %[x], a0" : [x] "=r" (res));

    return res;
}

int sem_wait(sem_t id){
    uint64 res;
    __asm__ volatile ("mv a1, %[x]" : : [x] "r" (id));
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x23));
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %[x], a0" : [x] "=r" (res));

    return res;
}

int sem_signal(sem_t id){
    uint64 res;
    __asm__ volatile ("mv a1, %[x]" : : [x] "r" (id));
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x24));
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %[x], a0" : [x] "=r" (res));

    return res;
}

char getc() {
    char res;
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x41));
    __asm__ volatile ("ecall");

    __asm__ volatile ("mv %[x], a0" : [x] "=r" (res));
    return res;
}

void putc(char c) {
    __asm__ volatile ("mv a1, %[x]" : : [x] "r" (c));
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x42));
    __asm__ volatile ("ecall");
}

int time_sleep(time_t time ) {
    return 0;
}

void userModeCall(){
    __asm__ volatile ("mv a0, %[x]" : : [x] "r" (0x50));
    __asm__ volatile ("ecall");
}