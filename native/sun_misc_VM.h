//
// Created by 14394 on 2020/2/27.
//

#ifndef AUTOMAN_JVM_SUN_MISC_VM_H
#define AUTOMAN_JVM_SUN_MISC_VM_H

#include <list>
#include "../runtime/oop.h"

using std::list;

void JVM_Initialize(list<Oop*>& _stack);

void* sun_misc_vm_search_method(const wstring& signature);


#endif //AUTOMAN_JVM_SUN_MISC_VM_H
