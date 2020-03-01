//
// Created by 14394 on 2020/2/26.
//

#include "../automan_jvm.h"
#include "java_lang_Package.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include "native.h"
#include "java_lang_String.h"
#include "../utils/utils.h"

static unordered_map<wstring, void*> methods = {
        {L"getSystemPackage0:(" STR ")" STR,								(void *)&JVM_GetSystemPackage0},
};

void JVM_GetSystemPackage0(list<Oop *> & _stack){		// static				// TODO:...
    InstanceOop *str = (InstanceOop *)_stack.front();	_stack.pop_front();
    _stack.push_back(nullptr);
}

void *java_lang_package_search_method(const wstring & signature)
{
    auto iter = methods.find(signature);
    if (iter != methods.end()) {
        return (*iter).second;
    }
    return nullptr;
}