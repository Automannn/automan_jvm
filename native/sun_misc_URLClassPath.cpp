//
// Created by 14394 on 2020/2/27.
//

#include "../automan_jvm.h"
#include "sun_misc_URLClassPath.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include "native.h"

static unordered_map<wstring, void*> methods = {
        {L"getLookupCacheURLs:(" JCL ")[Ljava/net/URL;",				(void *)&JVM_GetLookupCacheURLs},
};

void JVM_GetLookupCacheURLs(list<Oop *> & _stack){		// static
    _stack.push_back(nullptr);
}

void *sun_misc_urlClassPath_search_method(const wstring & signature)
{
    auto iter = methods.find(signature);
    if (iter != methods.end()) {
        return (*iter).second;
    }
    return nullptr;
}