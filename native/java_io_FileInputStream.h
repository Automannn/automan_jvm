//
// Created by 14394 on 2020/2/26.
//

#ifndef AUTOMAN_JVM_JAVA_IO_FILEINPUTSTREAM_H
#define AUTOMAN_JVM_JAVA_IO_FILEINPUTSTREAM_H

#include <list>
#include "../runtime/oop.h"

using std::list;

void JVM_FIS_InitIDs(list<Oop*>& _stack);
void JVM_Open0(list<Oop*>& _stack);
void JVM_ReadBytes(list<Oop*>& _stack);
void JVM_Close0(list<Oop*>& _stack);

void* java_io_fileInputStream_search_method(const wstring& signature);
#endif //AUTOMAN_JVM_JAVA_IO_FILEINPUTSTREAM_H
