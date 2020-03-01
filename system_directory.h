//
// Created by 14394 on 2020/2/24.
//

#ifndef AUTOMAN_JVM_SYSTEM_DIRECTORY_H
#define AUTOMAN_JVM_SYSTEM_DIRECTORY_H

#include <winsock2.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <windows.h>
#include "utils/lock.h"

using std::wstring;
using std::unordered_map;
using std::shared_ptr;

class Klass;

extern Lock system_classmap_lock;

extern  unordered_map<wstring,Klass*> system_classmap; //  java/lang/Object.class


#endif //AUTOMAN_JVM_SYSTEM_DIRECTORY_H
