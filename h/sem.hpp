#ifndef SEM_HPP
#define SEM_HPP

#include "../h/syscall_c.hpp"
#include "../h/list.hpp"

class Sem{

public:
    Sem (int init=1) {
        this->value = init;
        mutexClosed = true;
    }
    ~Sem();
    static Sem* open(unsigned int);
    int close();
    int signal();
    int wait();
    int try_wait();
    int timed_wait();
    bool isOpen() const { return mutexClosed; }
    void setOpen(bool o) { this->mutexClosed = o; }

private:
    int value;
    List<TCB> blockedList;
    bool mutexClosed;

};


#endif //SEM_HPP
