//
// Created by 14394 on 2020/2/26.
//

#ifndef AUTOMAN_JVM_JAVA_LANG_CLASSLOADER_H
#define AUTOMAN_JVM_JAVA_LANG_CLASSLOADER_H

#include <list>
#include "../runtime/oop.h"

using std::list;

void JVM_FindLoadedClass(list<Oop*>& _stack);
void JVM_FindBootStrapClass(list<Oop*>& _stack);
void JVM_DefineClass1(list<Oop*>& _stack);

void* java_lang_classLoader_search_method(const wstring& signature);


#endif //AUTOMAN_JVM_JAVA_LANG_CLASSLOADER_H
