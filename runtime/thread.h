//
// Created by 14394 on 2020/2/23.
//

#ifndef AUTOMAN_JVM_THREAD_H
#define AUTOMAN_JVM_THREAD_H

#include <unordered_map>
#include <memory>
#include <iostream>
#include <cassert>
#include <signal.h>
#include <errno.h>
#include <utility>
#include <tuple>
#include "../utils/lock.h"

using std::tuple;
using std::make_tuple;
using std::unordered_map;

class vm_thread;
class InstanceOop;
class GC;

class ThreadTable{
    friend GC;
private:
   static Lock& get_lock();
public:
    static unordered_map<DWORD,tuple<int,InstanceOop*,vm_thread*>>& get_thread_table();

public:
    //TODO: 这里的 tid 设置为 HANDLE 还是  DWORD呢？
    static void add_a_thread(DWORD tid,InstanceOop* _thread,vm_thread* t);
    static void remove_a_thread(DWORD tid);
    static int get_threadno(DWORD tid);
    static bool is_in(DWORD tid);
    static InstanceOop* get_a_thread(DWORD tid);
    static bool detect_thread_death(DWORD tid);
    static void print_table();
    static void kill_all_except_main_thread(DWORD main_tid);
    static int size(){return get_thread_table().size();}

public:
    static void wake_up_all_thread_force(); //after gc
};

#endif //AUTOMAN_JVM_THREAD_H
