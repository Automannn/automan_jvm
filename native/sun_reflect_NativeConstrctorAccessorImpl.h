//
// Created by 14394 on 2020/2/27.
//

#ifndef AUTOMAN_JVM_SUN_REFLECT_NATIVECONSTRCTORACCESSORIMPL_H
#define AUTOMAN_JVM_SUN_REFLECT_NATIVECONSTRCTORACCESSORIMPL_H

#include "../runtime/oop.h"
#include <list>

using std::list;

void JVM_NewInstanceFromConstructor(list<Oop*>& _stack);

void* sun_reflect_nativeConstructorAccessorImpl_search_method(const wstring& signature);

#endif //AUTOMAN_JVM_SUN_REFLECT_NATIVECONSTRCTORACCESSORIMPL_H
