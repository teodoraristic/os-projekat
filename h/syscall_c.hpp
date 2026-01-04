#ifndef SYSCALL_C_HPP
#define SYSCALL_C_HPP

#include "../lib/hw.h"

class TCB;
class Sem;

typedef TCB* thread_t;
typedef Sem* sem_t;

void* mem_alloc(size_t size);
int mem_free(void* ptr);
size_t mem_get_free_space();
size_t mem_get_largest_free_block();

int thread_create(thread_t* handle, void (*start_routine)(void*), void*);
int thread_exit();
void thread_dispatch();

int getThreadId();
void thread_join(thread_t handle);

/*void thread_pair(thread_t t1, thread_t t2);
void thread_sync();
void set_max_threads(int max);

void thread_send(thread_t handle, char* message);
char* thread_receive();*/

int sem_open(sem_t* handle, unsigned init);

int sem_close(sem_t handle);

int sem_wait(sem_t id);

int sem_signal(sem_t id);

typedef unsigned long time_t;

int time_sleep(time_t time);

const int EOF = -1;

char getc();

void putc(char c);

void userModeCall();

#endif //SYSCALL_C_HPP
