//
// Created by 14394 on 2020/2/27.
//

#include "sun_misc_VM.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include "../automan_jvm.h"
#include "native.h"

static unordered_map<wstring, void*> methods = {
        {L"initialize:()V",				(void *)&JVM_Initialize},
};

void JVM_Initialize(list<Oop *> & _stack){		// static
    // initialize jdk message. I set do nothing.
}

void *sun_misc_vm_search_method(const wstring & signature)
{
    auto iter = methods.find(signature);
    if (iter != methods.end()) {
        return (*iter).second;
    }
    return nullptr;
}