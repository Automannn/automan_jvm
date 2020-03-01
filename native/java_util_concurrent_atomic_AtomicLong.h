//
// Created by 14394 on 2020/2/27.
//

#ifndef AUTOMAN_JVM_JAVA_UTIL_CONCURRENT_ATOMIC_ATOMICLONG_H
#define AUTOMAN_JVM_JAVA_UTIL_CONCURRENT_ATOMIC_ATOMICLONG_H

#include <list>
#include "../runtime/oop.h"
using std::list;

void JVM_VMSupportsCS8(list<Oop*> _stack);

void * java_util_concurrent_atomic_atomicLong_search_method(const wstring& signature);


#endif //AUTOMAN_JVM_JAVA_UTIL_CONCURRENT_ATOMIC_ATOMICLONG_H
