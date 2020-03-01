//
// Created by 14394 on 2020/2/26.
//

#ifndef AUTOMAN_JVM_JAVA_IO_UNIXFILESYSTEM_H
#define AUTOMAN_JVM_JAVA_IO_UNIXFILESYSTEM_H

#include "../runtime/oop.h"
#include <list>

using std::list;

void JVM_UFS_InitIDs(list<Oop*>& _stack);
void JVM_Canonicalize0(list<Oop*>& _stack);
void JVM_GetBooleanAttributes0(list<Oop*>& _stack);

void* java_io_unixFileSystem_search_method(const wstring& signature);


#endif //AUTOMAN_JVM_JAVA_IO_UNIXFILESYSTEM_H
