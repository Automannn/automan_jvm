//
// Created by 14394 on 2020/2/23.
//

#ifndef AUTOMAN_JVM_GC_H
#define AUTOMAN_JVM_GC_H

#include <unordered_map>
#include <utility>
#include "../utils/lock.h"
#include <windows.h>

using std::unordered_map;
using std::pair;
using std::make_pair;

class vm_thread;
class Oop;
class Klass;

class GC{
public:
     static HANDLE gc_cond ;
     static HANDLE gc_cond_mutes;
private:
    static void recursive_add_oop_and_its_inner_oops_and_modify_pointers_by_the_way(Oop* const& origin_oop, unordered_map<Oop *,Oop*>& new_oop_map);

    static void klass_inner_oop_gc(Klass* klass,unordered_map<Oop*,Oop*>& new_oop_map);

public:
    static Lock& gc_lock(){
        static Lock gc_lock;
        return gc_lock;
    }

    static bool& gc(){
        static bool gc= false;
        return gc;
    }

public:
    static bool init_gc();
    static void detect_ready();
    static unsigned int * gc_thread(void *);
    static void system_gc();
    static void set_safepoint_here(vm_thread*);
    static void signal_all_patch();
    static void cancel_gc_thread();
};



#endif //AUTOMAN_JVM_GC_H
