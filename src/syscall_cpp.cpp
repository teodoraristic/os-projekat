#include "../h/syscall_cpp.hpp"
#include "../h/syscall_c.hpp"
#include "../h/tcb.hpp"

Thread::Thread() {
    thread_create(&myHandle, nullptr, nullptr);
    myHandle->cppThread = this;
}

Thread::Thread(void(*body)(void*), void*arg) {
    this->body = body;
    this->arg = arg;
    thread_create(&myHandle, body, arg);
    myHandle->cppThread = this;
}

int Thread::start() {
    if (!myHandle) return -1;

    /*if (!myHandle->isBlocked()) {
        Scheduler::put(myHandle);
    }*/
    Scheduler::put(myHandle);
    return 0;
}
void Thread::run(){}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t time) {
    return 0;
}

int Thread::getThreadId() {
    return ::getThreadId();
}

int Thread::join() {
    if (!myHandle) return -1;
    ::thread_join(myHandle);
    return 0;
}

/*void Thread::pair(Thread *t1, Thread *t2) {
    if (t1 && t2 && t1->myHandle && t2->myHandle) {
        thread_pair(t1->myHandle, t2->myHandle);
    }
}

void Thread::sync() {
    thread_sync();
}

void Thread::setMaximumThreads(int maxThreads) {
    ::set_max_threads(maxThreads);
}

void Thread::send(char* message) {
    if (!myHandle) return;
    ::thread_send(myHandle, message);
}

char* Thread::receive() {
    return ::thread_receive();
}*/

Thread::~Thread() {
    delete myHandle;
}

Semaphore::Semaphore(unsigned int init) {
    sem_open( &myHandle, init );
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

Semaphore::~Semaphore() {
    sem_close( myHandle );
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}


