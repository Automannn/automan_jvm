//
// Created by 14394 on 2020/2/27.
//

#ifndef AUTOMAN_JVM_SUN_MISC_URLCLASSPATH_H
#define AUTOMAN_JVM_SUN_MISC_URLCLASSPATH_H

#include <list>
#include "../runtime/oop.h"

using std::list;

void JVM_GetLookupCacheURLs(list<Oop*>& _stack);

void* sun_misc_urlClassPath_search_method(const wstring& signature);

#endif //AUTOMAN_JVM_SUN_MISC_URLCLASSPATH_H
