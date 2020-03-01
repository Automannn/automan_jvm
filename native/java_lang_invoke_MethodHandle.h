//
// Created by 14394 on 2020/2/26.
//

#ifndef AUTOMAN_JVM_JAVA_LANG_INVOKE_METHODHANDLE_H
#define AUTOMAN_JVM_JAVA_LANG_INVOKE_METHODHANDLE_H

#include "../runtime/oop.h"
#include <list>

using std::list;

void JVM_Invoke(list<Oop*>& _stack);
void JVM_InvokeBasic(list<Oop*>& _stack);
void JVM_InvokeExact(list<Oop*>& _stack);

void* java_lang_invoke_methodHandle_search_method(const wstring& str);

class vm_thread;

void argument_unboxing(Method* method,list<Oop*>& args);
InstanceOop* return_val_boxing(Oop* basic_type_oop,vm_thread* thread,const wstring& return_type);

#endif //AUTOMAN_JVM_JAVA_LANG_INVOKE_METHODHANDLE_H
