//
// Created by 14394 on 2020/2/23.
//

#ifndef AUTOMAN_JVM_OS_H
#define AUTOMAN_JVM_OS_H

#include <c++/cstdint>

int get_cpu_nums();

//CAS，from x86 assembly, and this code is from openjdk;
inline int cmpxchg(int exchange_value, volatile int *dest,int compare_value){
    int mp = get_cpu_nums()>1?get_cpu_nums():0;
    __asm__ volatile("cmp $0, %4; je 1f; lock; 1: cmpxchgl %1,(%3)"
    : "=a"(exchange_value)
    : "r"(exchange_value), "a"(compare_value), "r"(dest), "r"(mp)
    : "cc", "memory");

    return exchange_value;
}

inline long cmpxchg(long exchange_value, volatile long *dest, long compare_value)
{
    int mp = get_cpu_nums() > 1 ? get_cpu_nums() : 0;
    __asm__ volatile("cmp $0, %4; je 1f; lock; 1: cmpxchgq %1,(%3)"
    : "=a"(exchange_value)
    : "r"(exchange_value), "a"(compare_value), "r"(dest), "r"(mp)
    : "cc", "memory");
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
