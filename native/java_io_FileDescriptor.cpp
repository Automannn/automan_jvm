//
// Created by 14394 on 2020/2/23.
//

#include "../automan_jvm.h"
#include "java_io_FileDescriptor.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include "native.h"

static unordered_map<wstring, void*> methods = {
        {L"initIDs:()V",				(void *)&JVM_FD_InitIDs},
};

void JVM_FD_InitIDs(list<Oop *> & _stack){		// static
    // do nothing
}

void *java_io_fileDescriptor_search_method(const wstring & signature)
{
    auto iter = methods.find(signature);
    if (iter != methods.end()) {
        return (*iter).second;
    }
    return nullptr;
}