//
// Created by 14394 on 2020/2/27.
//

#ifndef AUTOMAN_JVM_JAVA_LANG_THROWABLE_H
#define AUTOMAN_JVM_JAVA_LANG_THROWABLE_H

#include <list>
#include "../runtime/oop.h"

using std::list;

void JVM_FillInStackTrace(list<Oop*>& _stack);
void JVM_GetStackTraceDepth(list<Oop*>& _stack);
void JVM_GetStackTraceElement(list<Oop*>& _stack);

void* java_lang_throwable_search_method(const wstring& str);


#endif //AUTOMAN_JVM_JAVA_LANG_THROWABLE_H
