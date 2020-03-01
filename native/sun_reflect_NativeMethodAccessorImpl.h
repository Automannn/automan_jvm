//
// Created by 14394 on 2020/2/27.
//

#ifndef AUTOMAN_JVM_SUN_REFLECT_NATIVEMETHODACCESSORIMPL_H
#define AUTOMAN_JVM_SUN_REFLECT_NATIVEMETHODACCESSORIMPL_H

#include <list>
#include "../runtime/oop.h"

using std::list;

void JVM_Invoke0(list<Oop*>& _stack);

void* sun_reflect_nativeMethodAccessorImpl_search_method(const wstring& signature);


#endif //AUTOMAN_JVM_SUN_REFLECT_NATIVEMETHODACCESSORIMPL_H
