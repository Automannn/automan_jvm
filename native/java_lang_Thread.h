//
// Created by 14394 on 2020/2/27.
//

#ifndef AUTOMAN_JVM_JAVA_LANG_THREAD_H
#define AUTOMAN_JVM_JAVA_LANG_THREAD_H

#include "../runtime/oop.h"
#include <list>

using std::list;

enum ThreadPriority{
    NoPriority = -1,
    MinPriority = 1,
    NormPriority = 5,
    NearMaxPriority= 9,
    MaxPriority = 10,
    CriticalPriority = 11
};

void JVM_StartThread(list<Oop*>& _stack);
void JVM_StopThread(list<Oop*>& _stack);
void JVM_IsThreadAlive(list<Oop*>& _stack);
void JVM_SuspendThread(list<Oop*>& _stack);
void JVM_ResumeThread(list<Oop*>& _stack);
void JVM_SetThreadPriority(list<Oop*>& _stack);
void JVM_Yield(list<Oop*>& _stack);
void JVM_Sleep(list<Oop*>& _stack);
void JVM_CurrentThread(list<Oop*>& _stack);
void JVM_CountStackFrames(list<Oop*>& _stack);
void JVM_Interrupt(list<Oop*>& _stack);
void JVM_IsInterrupted(list<Oop*>& _stack);
void JVM_HoldsLock(list<Oop*>& _stack);
void JVM_GetAllThreads(list<Oop*>& _stack);
void JVM_DumpThreads(list<Oop*>& _stack);
void JVM_SetNativeThreadName(list<Oop*>& _stack);

void* java_lang_thread_search_method(const wstring& str);


#endif //AUTOMAN_JVM_JAVA_LANG_THREAD_H
