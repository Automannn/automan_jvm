//
// Created by 14394 on 2020/2/26.
//

#include "../automan_jvm.h"
#include "java_lang_Float.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include "native.h"

static unordered_map<wstring, void*> methods = {
        {L"floatToRawIntBits:(F)I",				(void *)&JVM_FloatToRawIntBits},
};

// this method, I simplified it to just run the `run()` method.
void JVM_FloatToRawIntBits(list<Oop *> & _stack){		// static
    float v = ((FloatOop *)_stack.front())->value;	_stack.pop_front();
    union {
        int i;
        float f;
    } u;
    u.f = (float)v;		// 见 openjdk: share/native/java/lang/Float.c::Java_java_lang_Float_floatToRawIntBits()
#ifdef DEBUG
    sync_wcout{} << "(DEBUG) [use bits] to convert float: [" << u.f << "] to int: [" << u.i << "] and return the int value." << std::endl;
#endif
    _stack.push_back(new IntOop(u.i));
}

void *java_lang_float_search_method(const wstring & signature)
{
    auto iter = methods.find(signature);
    if (iter != methods.end()) {
        return (*iter).second;
    }
    return nullptr;
}