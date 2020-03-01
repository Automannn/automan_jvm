//
// Created by 14394 on 2020/2/26.
//

#ifndef AUTOMAN_JVM_JAVA_LANG_REFLECT_ARRAY_H
#define AUTOMAN_JVM_JAVA_LANG_REFLECT_ARRAY_H

#include <list>
#include "../runtime/oop.h"

using std::list;

void JVM_NewArray(list<Oop*>& _stack);

void* java_lang_reflect_array_search_method(const wstring& str);


#endif //AUTOMAN_JVM_JAVA_LANG_REFLECT_ARRAY_H
