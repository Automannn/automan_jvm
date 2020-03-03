//
// Created by 14394 on 2020/2/26.
//

#ifndef AUTOMAN_JVM_JAVA_IO_FILESYSTEM_H
#define AUTOMAN_JVM_JAVA_IO_FILESYSTEM_H

#include <list>
#include "../runtime/oop.h"

using std::list;

void JVM_GetLength(list<Oop*>& _stack);

void JVM_GetBooleanAttributes(list<Oop *> & _stack);

void* java_io_fileSystem_search_method(const wstring& signature);


#endif //AUTOMAN_JVM_JAVA_IO_FILESYSTEM_H
