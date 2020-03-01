//
// Created by 14394 on 2020/2/26.
//

#ifndef AUTOMAN_JVM_JAVA_LANG_OBJECT_H
#define AUTOMAN_JVM_JAVA_LANG_OBJECT_H

#include "../runtime/oop.h"
#include <list>

using std::list;

void JVM_IHashCode(list<Oop*>& _stack);
void JVM_MonitorWait(list<Oop*>& _stack);
void JVM_MonitorNotify(list<Oop*>& _stack);
void JVM_MonitorNotifyAll(list<Oop*>& _stack);
void JVM_Clone(list<Oop*>& _stack);
void Java_java_lang_object_getClass(list<Oop*>& _stack);

void* java_lang_object_search_method(const wstring& signature);

#endif //AUTOMAN_JVM_JAVA_LANG_OBJECT_H
