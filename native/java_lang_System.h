//
// Created by 14394 on 2020/2/27.
//

#ifndef AUTOMAN_JVM_JAVA_LANG_SYSTEM_H
#define AUTOMAN_JVM_JAVA_LANG_SYSTEM_H

#include "../runtime/oop.h"
#include <list>

using std::list;

void JVM_CurrentTimeMillis(list<Oop *> & _stack);
void JVM_NanoTime(list<Oop *> & _stack);
void JVM_ArrayCopy(list<Oop *> & _stack);
void JVM_IdentityHashCode(list<Oop *> & _stack);
void JVM_InitProperties(list<Oop *> & _stack);
void JVM_MapLibraryName(list<Oop *> & _stack);
void JVM_SetIn0(list<Oop *> & _stack);
void JVM_SetOut0(list<Oop *> & _stack);
void JVM_SetErr0(list<Oop *> & _stack);

void *java_lang_system_search_method(const wstring & str);

#endif //AUTOMAN_JVM_JAVA_LANG_SYSTEM_H
