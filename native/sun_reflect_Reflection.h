//
// Created by 14394 on 2020/2/27.
//

#ifndef AUTOMAN_JVM_SUN_REFLECT_REFLECTION_H
#define AUTOMAN_JVM_SUN_REFLECT_REFLECTION_H

#include "../runtime/oop.h"
#include <list>

using std::list;

void JVM_GetCallerClass(list<Oop*>& _stack);
void JVM_GetClassAccessFlags(list<Oop*>& _stack);

void* sun_reflect_reflection_search_method(const wstring& signature);


#endif //AUTOMAN_JVM_SUN_REFLECT_REFLECTION_H
