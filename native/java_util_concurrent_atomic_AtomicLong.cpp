//
// Created by 14394 on 2020/2/27.
//

#include "../automan_jvm.h"
#include "java_util_concurrent_atomic_AtomicLong.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include "native.h"

static unordered_map<wstring, void*> methods = {
        {L"VMSupportsCS8:()Z",				(void *)&JVM_VMSupportsCS8},
};

void JVM_VMSupportsCS8(list<Oop*> _stack){		// static
    // x86 always returns true.
    _stack.push_back(new IntOop(true));
}

void *java_util_concurrent_atomic_atomicLong_search_method(const wstring & signature)
{
    auto iter = methods.find(signature);
    if (iter != methods.end()) {
        return (*iter).second;
    }
    return nullptr;
}