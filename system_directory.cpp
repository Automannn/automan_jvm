//
// Created by 14394 on 2020/2/24.
//

#include "system_directory.h"
#include "runtime/kclass.h"

Lock system_classmap_lock;

unordered_map<wstring,Klass*> system_classmap;
