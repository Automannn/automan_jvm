//
// Created by 14394 on 2020/3/2.
//

#ifndef AUTOMAN_JVM_SUN_IO_WIN32ERRORMODE_H
#define AUTOMAN_JVM_SUN_IO_WIN32ERRORMODE_H

#include <list>
#include "../runtime/oop.h"

using std::list;

void JVM_SetErrorMode(list<Oop*>& _stack);

void *sun_io_Win32ErrorMode_search_method(const wstring& signature);


#endif //AUTOMAN_JVM_SUN_IO_WIN32ERRORMODE_H
