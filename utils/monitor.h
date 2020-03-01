//
// Created by 14394 on 2020/2/23.
//

#ifndef AUTOMAN_JVM_MONITOR_H
#define AUTOMAN_JVM_MONITOR_H

#include <time.h>
#include <windows.h>
#include "noncopyable.h"

// this class object, will be embedded into [Oop's header].
class Monitor:noncopyable{
private:
    HANDLE _mutex;
    HANDLE _cond;
public:
    Monitor() {
        _mutex = CreateMutex(NULL,FALSE,NULL);
        _cond = CreateEvent(NULL,FALSE,FALSE,NULL);
    }
    void enter() {
        WaitForSingleObject(_mutex,INFINITE);
    }
    void wait() {
        WaitForSingleObject(_cond,INFINITE);
    }
    void wait(long timesec) {
        WaitForSingleObject(_cond,timesec);
    }
    void notify() {
        SetEvent(_cond);
    }
    void notify_all() {
        SetEvent(_cond);
    }
    void leave() {
        ReleaseMutex(_mutex);
    }
    void force_unlock_when_athrow() {
        ReleaseSemaphore(_mutex);
    }
    ~Monitor() {
        CloseHandle(_mutex);
        CloseHandle(_cond);
    }
};

#endif //AUTOMAN_JVM_MONITOR_H
