//
// Created by 14394 on 2020/2/23.
//

#ifndef AUTOMAN_JVM_LOCK_H
#define AUTOMAN_JVM_LOCK_H

#include <windows.h>
#include "noncopyable.h"

class GC;

class Lock: public noncopyable{
    friend GC;
private:
    HANDLE mutex;
public:
    Lock();
    void lock();
    void unlock();
    ~Lock();
};

class LockGuard {		// RAII
private:
    Lock & lock;
public:
    LockGuard(Lock & lock) : lock(lock){ lock.lock(); }
    ~LockGuard() { lock.unlock(); }
};



#endif //AUTOMAN_JVM_LOCK_H
