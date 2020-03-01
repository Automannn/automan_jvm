//
// Created by 14394 on 2020/2/24.
//

#ifndef AUTOMAN_JVM_AUTOMAN_JVM_H
#define AUTOMAN_JVM_AUTOMAN_JVM_H

#include <string>
#include <list>
#include <vector>
#include <memory>
#include <winsock2.h>
#include "utils/lock.h"
#include "runtime/bytecodeEngine.h"
#include "utils/synchronize_wcout.h"
#include "runtime/gc.h"

class vm_thread;
class automan_jvm;

struct temp{
    vm_thread* thread;
    std::list<Oop*>* arg;
    InstanceOop* cur_thread_obj;
    volatile bool should_be_stop_first= false;
    volatile bool the_first_wait_executed = false;
};

extern Lock thread_num_lock;
extern int all_thread_num;

enum thread_state{
    Running,
    Waiting,
    Death,
};

class ThreadTable;

class vm_thread{
    friend BytecodeEngine;
    friend GC;
    friend ThreadTable;

private:
    temp p;
    HANDLE tid;
    thread_state state = Running;

    int monitor_num = 0;

    Method* method;
    std::list<Oop*> arg;
    list<StackFrame> vm_stack;
    uint8_t* pc;
    int thread_no;

public:
    vm_thread(Method* method,const std::list<Oop*>& arg): method(method),arg(arg),pc(0),tid(0){
        LockGuard lg(thread_num_lock);
#ifdef DEBUG
        sync_wcout{} << "[*]this thread_no is [" << all_thread_num << "]:" << std::endl;		// delete
#endif
        this->thread_no = all_thread_num++;
    }

public:
    void launch(InstanceOop* = nullptr);
    void start(std::list<Oop*>& arg);
    Oop* execute();
    Oop* add_frame_and_execute(Method* new_method,const std::list<Oop*>& list);
    MirrorOop* get_caller_class_CallerSensitive();
    void init_and_do_main();
    ArrayOop* get_stack_trace();
    int get_stack_size(){return vm_stack.size();}
    void set_exception_at_last_second_frame();

public:
    void set_state(thread_state s){state  = s;}
    void monitor_inc(){this->monitor_num++;}
    void monitor_dec(){this->monitor_num--;}
    int get_monitor_num(){return this->monitor_num;}
    bool is_waited_for_child(){return p.the_first_wait_executed;}
    HANDLE get_tid(){return tid;}
};

class automan_jvm{
    friend BytecodeEngine;
    friend vm_thread;

private:
    static bool& inited(){
        static bool inited = false;
        return inited;
    }

    static wstring& main_class_name(){
        static wstring main_class_name;
        return main_class_name;
    }

    static vector<wstring>& argv(){
        static vector<wstring> argv;
        return argv;
    }

public:
    static HANDLE& gc_thread(){
        //todo：新建线程
        static HANDLE  gc_thread_handle;
        return gc_thread_handle;
    }

    static int& thread_num(){
        static int thread_num =0;
        return thread_num;
    }

    static Lock& num_lock(){
        static Lock num_lock;
        return num_lock;
    }

    static Lock& lock(){
        static Lock lock;
        return lock;
    }

    static list<vm_thread>& threads(){
        static list<vm_thread> threads;
        return threads;
    }

    static void run(const wstring& main_class_name,const vector<wstring>& argv);
    static void end();
};

extern HANDLE _all_thread_wait_mutex;
extern HANDLE _all_thread_wait_cond;


void wait_cur_thread(vm_thread* thread);
void wait_cur_thread_and_set_bit(volatile bool*,vm_thread *);
void signal_one_thread();
void signal_all_thread();

#endif //AUTOMAN_JVM_AUTOMAN_JVM_H
