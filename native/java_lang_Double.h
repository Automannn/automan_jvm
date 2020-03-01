//
// Created by 14394 on 2020/2/26.
//

#ifndef AUTOMAN_JVM_JAVA_LANG_DOUBLE_H
#define AUTOMAN_JVM_JAVA_LANG_DOUBLE_H

#include "../runtime/oop.h"
#include <list>

using std::list;

void JVM_DoubleToRawLongBits(list<Oop*>& _stack);
void JVM_LongBitsToDouble(list<Oop*>& _stack);

void* java_lang_double_search_method(const wstring& signature);


#endif //AUTOMAN_JVM_JAVA_LANG_DOUBLE_H
