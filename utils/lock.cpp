//
// Created by 14394 on 2020/2/23.
//

#include "lock.h"

Lock::Lock(){
    mutex = CreateMutex(NULL,FALSE,NULL);
}

void Lock::lock() {
    WaitForSingleObject(mutex,INFINITE);
}

void Lock::unlock() {
    ReleaseMutex(mutex);
}

Lock::~Lock() {
    CloseHandle(mutex);
}