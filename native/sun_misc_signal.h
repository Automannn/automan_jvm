//
// Created by 14394 on 2020/2/27.
//

#ifndef AUTOMAN_JVM_SUN_MISC_SIGNAL_H
#define AUTOMAN_JVM_SUN_MISC_SIGNAL_H

#include <list>
#include "../runtime/oop.h"

using std::list;

void JVM_FindSignal(list<Oop*>& _stack);
void JVM_Handle0(list<Oop*>& _stack);

void *sun_misc_signal_search_method(const wstring& signature);

#endif //AUTOMAN_JVM_SUN_MISC_SIGNAL_H
