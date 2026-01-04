#ifndef SYSCALL_CPP_HPP
#define SYSCALL_CPP_HPP

#include "../h/syscall_c.hpp"

class Thread {
public:
    Thread(void (*body)(void*), void* arg);

    virtual ~Thread();

    int start();
    static void dispatch();
    static int sleep(time_t time);

    static int getThreadId();
    int join();

    /*static void pair(Thread* t1, Thread* t2);
    void sync();
    static void setMaximumThreads(int maxThreads);

    void send(char* message);
    static char* receive();*/

protected:
    Thread();
    virtual void run();

private:
    thread_t myHandle;
    void (*body)(void*);
    void* arg;
    friend class TCB;


    static void runWrapper(void* thread) {
        if(thread) {
            ((Thread*)thread)->run();
        }
    }

};

class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();

private:
    sem_t myHandle;
};

class Console {
public:
    static char getc ();
    static void putc (char);
};


#endif //SYSCALL_CPP_HPP
