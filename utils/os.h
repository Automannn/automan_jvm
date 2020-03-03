//
// Created by 14394 on 2020/2/23.
//

#ifndef AUTOMAN_JVM_OS_H
#define AUTOMAN_JVM_OS_H

#include <windows.h>
#include <c++/cstdint>
#include <winnt.h>

int get_cpu_nums();

//CAS，from x86 assembly, and this code is from openjdk;
inline int cmpxchg(int exchange_value, volatile int *dest,int compare_value){
    InterlockedCompareExchange(reinterpret_cast<volatile long *>(dest), exchange_value, compare_value);
    return exchange_value;
}

inline long cmpxchg(long exchange_value, volatile long *dest, long compare_value)
{
    InterlockedCompareExchange(dest,exchange_value,compare_value);
    return exchange_value;
}

inline void fence()
{
    // barrier. I use boost::atomic//ops_gcc_x86.hpp's method.
    // openjdk says `mfence` is always expensive...
    __asm__ volatile (
#ifdef __x86_64__
    "mfence;"
#else
    "lock; addl $0, (%%esp);"
#endif
    :::"memory"
    );
}

// from openjdk: OrderAccess
inline void release()
{
    // Avoid hitting the same cache-line from different threads.
    volatile int local_dummy = 0;
}

// from openjdk: OrderAccess
inline void acquire()
{
    volatile intptr_t local_dummy;
    __asm__ volatile ("movq 0(%%rsp), %0" : "=r" (local_dummy) : : "memory");
}



#endif //AUTOMAN_JVM_OS_H