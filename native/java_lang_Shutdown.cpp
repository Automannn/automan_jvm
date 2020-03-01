//
// Created by 14394 on 2020/2/26.
//

#include "java_lang_Shutdown.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include "../automan_jvm.h"
#include "native.h"

static unordered_map<wstring, void*> methods = {
        {L"halt0:(I)V",				(void *)&JVM_Halt0},
};

void JVM_Halt0(list<Oop *> & _stack){		// static

    int exitcode = ((IntOop *)_stack.front())->value;	_stack.pop_front();
    // TODO: fully gc.
    //todo： 在 windows中，主线程退出，则子线程也会退出，需要注意
    BytecodeEngine::main_thread_exception(exitcode);
}

