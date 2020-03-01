//
// Created by 14394 on 2020/2/27.
//

#ifndef AUTOMAN_JVM_SUN_MISC_UNSAFE_H
#define AUTOMAN_JVM_SUN_MISC_UNSAFE_H

#include <list>
#include "../runtime/oop.h"

using std::string;

void JVM_ArrayBaseOffset(list<Oop *> & _stack);
void JVM_ArrayIndexScale(list<Oop *> & _stack);
void JVM_AddressSize(list<Oop *> & _stack);
void JVM_ObjectFieldOffset(list<Oop *> & _stack);
void JVM_GetIntVolatile(list<Oop *> & _stack);
void JVM_CompareAndSwapInt(list<Oop *> & _stack);
void JVM_AllocateMemory(list<Oop *> & _stack);
void JVM_PutLong(list<Oop *> & _stack);
void JVM_GetByte(list<Oop *> & _stack);
void JVM_FreeMemory(list<Oop *> & _stack);
void JVM_GetObjectVolatile(list<Oop *> & _stack);
void JVM_CompareAndSwapObject(list<Oop *> & _stack);
void JVM_CompareAndSwapLong(list<Oop *> & _stack);
void JVM_ShouldBeInitialized(list<Oop *> & _stack);
void JVM_DefineAnonymousClass(list<Oop *> & _stack);
void JVM_EnsureClassInitialized(list<Oop *> & _stack);
void JVM_DefineClass(list<Oop *> & _stack);
void JVM_PutObjectVolatile(list<Oop *> & _stack);
void JVM_StaticFieldOffset(list<Oop *> & _stack);
void JVM_StaticFieldBase(list<Oop *> & _stack);
void JVM_PutObject(list<Oop *> & _stack);


void *sun_misc_unsafe_search_method(const wstring & str);

#endif //AUTOMAN_JVM_SUN_MISC_UNSAFE_H
