//
// Created by 14394 on 2020/2/26.
//

#ifndef AUTOMAN_JVM_JAVA_LANG_RUNTIME_H
#define AUTOMAN_JVM_JAVA_LANG_RUNTIME_H

#include <list>
#include "../runtime/oop.h"

using std::list;

void JVM_AvailableProcessors(list<Oop*>& _stack);

void* java_lang_runtime_search_method(const wstring& signature);


#endif //AUTOMAN_JVM_JAVA_LANG_RUNTIME_H
