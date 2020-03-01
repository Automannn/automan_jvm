//
// Created by 14394 on 2020/2/23.
//

#include "os.h"
#include <cassert>
#include <unistd.h>
#include <windows.h>

int get_cpu_nums(){
    SYSTEM_INFO mySysInfo;
    GetSystemInfo(&mySysInfo);
    static int cpu_nums = mySysInfo.dwNumberOfProcessors;		// global cache
    assert(cpu_nums != 0);
    return cpu_nums;
}