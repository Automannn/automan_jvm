//
// Created by 14394 on 2020/3/2.
//

#ifndef AUTOMAN_JVM_JAVA_IO_WINNTFILESYSTEM_H
#define AUTOMAN_JVM_JAVA_IO_WINNTFILESYSTEM_H

#include <list>
#include "../runtime/oop.h"

using std::list;

void JVM_InitIDs(list<Oop*>& _stack);
void JVM_WIN_Canonicalize0(list<Oop*>& _stack);
//void JVM_WIN_getLength(list<Oop*>& _stack);
void *java_io_WinNTFileSystem_search_method(const wstring& signature);


#endif //AUTOMAN_JVM_JAVA_IO_WINNTFILESYSTEM_H