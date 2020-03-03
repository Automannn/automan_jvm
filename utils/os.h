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
//x,target,expected  -->期望的是 交换。
inline int cmpxchg(int exchange_value, volatile long *dest,int compare_value){
    //dest 是 被比较的值，是一个指针。 如果 compare与 dest值相等，则dest的值用 exchange换掉， compare是用于比较的值
    //返回原来dest指向的值
    LONG ret= InterlockedCompareExchange(dest, exchange_value, compare_value);
    return ret;
}

inline long cmpxchg(long exchange_value, volatile long *dest, long compare_value)
{
    LONG ret =InterlockedCompareExchange(dest,exchange_value,compare_value);
    return ret;
}

inline PVOID cmpxchg(PVOID exchange_value, volatile long *dest, PVOID compare_value)
{
    //此时 dest中的内容存的应该为 一个指针
    //返回值的逻辑尚未进行验证，但是初始化的时候是可以通过的。
    LONG ret =InterlockedCompareExchange(dest,(long long)exchange_value,(long long)compare_value);
    return (PVOID)ret;
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