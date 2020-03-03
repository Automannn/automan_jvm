//
// Created by 14394 on 2020/2/23.
//

#ifndef AUTOMAN_JVM_JAVA_IO_FILEDESCRIPTOR_H
#define AUTOMAN_JVM_JAVA_IO_FILEDESCRIPTOR_H

#include "../runtime/oop.h"
#include <list>

using std::list;

void JVM_FD_InitIDs(list<Oop *>& _stack);

void JVM_FD_Set(list<Oop *>& _stack);

void* java_io_fileDescriptor_search_method(const wstring& signature);

#endif //AUTOMAN_JVM_JAVA_IO_FILEDESCRIPTOR_H
