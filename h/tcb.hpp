#ifndef TCB_HPP
#define TCB_HPP

#include "../lib/hw.h"
#include "../h/scheduler.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_cpp.hpp"

class Thread;

typedef TCB* thread_t;

class TCB {
public:

    using Body = void (*)(void*);
    static long gPid;
    static long tcbCount;

    ~TCB() {  MemoryAllocator::mem_free(stack); }

    bool isFinished() const { return finished; }
    void setFinished(const bool value){finished = value;}

    void block() { blocked = true; }
    void unblock() { blocked = false; }
    bool isBlocked() const { return blocked; }

    bool isMain() const { return main; }
    long getPid() const { return myPid; }

    static TCB *createThread(TCB::Body body, void* arg);
    static int getId();

    static void join(TCB* handle);

    /*static void pair(TCB* t1, TCB* t2);
    void sync();
    void setPartner(TCB* p)  {this->partner = p;};
    TCB* getPartner(){return this->partner;};
    bool isWaitingForSync() {return this->waitingForSync;}
    void setWaitingForSync(bool b) {this->waitingForSync = b;}

    static void SetMaximumThreads(int num_of_threads);

    static void send(TCB* handle, const char* message);
    static char* receive();
    void setMessage(const char* msg);
    char* getMessage(){return message;}
    bool hasMessage() const { return message != nullptr; }
    void clearMessage();
    
    static void ping(TCB* handle);
    void setPinged(bool p) {pinged = p;}
    bool isPinged() const {return pinged;}
    void dumpMemoryInfo();
    void addAllocation(void* ptr, size_t size);
    void removeAllocation(void* ptr);	


*/

    static void yield();
    static void dispatch();
    static int exit();


    static TCB* running;
    Thread* cppThread;

private:

    TCB(Body body, void* args):
        body(body),
        stack(new uint64[DEFAULT_STACK_SIZE]),
        context({(uint64) &threadWrapper,
                 stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0}),
        finished(false),
        blocked(false),
        args(args),
        main(body==nullptr),
        myPid(gPid++)/*,
        partner(nullptr),
        waitingForSync(false),
        allocatedMemory(nullptr)*/
    {tcbCount++;}

    struct Context {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    uint64 *stack;
    Context context;

    bool finished;
    bool blocked;
    void *args;
    bool main;
    long myPid;
   /* TCB* partner;
    bool waitingForSync;
    static int maxUserThreads;
    static int currentUserThreads;
    static List<TCB> waitingThreads;
    char* message = nullptr;
    bool messageWaiting = false;
    bool pinged = false;
    struct Allocation {
	void* address;
	size_t size;
	Allocation* next;
	Allocation(void* addr, size_t s) : address(addr), size(s), next(nullptr){}
    };
    Allocation* allocatedMemory;

*/

    friend class Riscv;

    static void threadWrapper();

    static void contextSwitch(Context* oldContext, Context* runningContext);


};
#endif //TCB_HPP
