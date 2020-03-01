//
// Created by 14394 on 2020/2/27.
//

#ifndef AUTOMAN_JVM_JAVA_SECURITY_ACCESSCONTROLLER_H
#define AUTOMAN_JVM_JAVA_SECURITY_ACCESSCONTROLLER_H

#include "../runtime/oop.h"
#include <list>

using std::list;

void JVM_DoPrivileged(list<Oop*>& _stack);
void JVM_GetStackAccessControlContext(list<Oop*>& _stack);

void *java_security_accesscontroller_search_method(const wstring& signature);


#endif //AUTOMAN_JVM_JAVA_SECURITY_ACCESSCONTROLLER_H
