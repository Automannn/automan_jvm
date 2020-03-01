//
// Created by 14394 on 2020/2/26.
//

#ifndef AUTOMAN_JVM_JAVA_IO_FILEOUTPUTSTREAM_H
#define AUTOMAN_JVM_JAVA_IO_FILEOUTPUTSTREAM_H

#include "../runtime/oop.h"
#include <list>

using std::list;

void JVM_FOS_InitIDs(list<Oop*>& _stack);
void JVM_WriteBytes(list<Oop*>& _stack);

void* java_io_fileOutputStream_search_method(const wstring& signature);

#endif //AUTOMAN_JVM_JAVA_IO_FILEOUTPUTSTREAM_H
