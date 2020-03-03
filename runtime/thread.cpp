//
// Created by 14394 on 2020/2/23.
//

#include "../automan_jvm.h"
#include "thread.h"
#include "../utils/synchronize_wcout.h"

Lock & ThreadTable::get_lock()
{
    static Lock lock;
    return lock;
}

unordered_map<DWORD, tuple<int, InstanceOop *, vm_thread *>> & ThreadTable::get_thread_table()
{
    static unordered_map<DWORD, tuple<int, InstanceOop *, vm_thread *>> thread_table;
    return thread_table;
}

void ThreadTable::add_a_thread(DWORD tid, InstanceOop *_thread, vm_thread *t)
{
    LockGuard lg(get_lock());

    // C++ 17 新增了一个 insert_or_assign() 比较好...QAQ
//	get_thread_table().insert(make_pair(tid, make_pair(get_thread_table().size(), _thread)));		// Override!!!! because tid maybe the same...?
    int number = get_thread_table().size();
    if (get_thread_table().insert(make_pair(tid, make_tuple(get_thread_table().size(), _thread, t))).second == false) {	// 如果原先已经插入了的话，那么就复用原先的 thread_no.
        number = std::get<0>(get_thread_table()[tid]);
    }
    //todo: 使用 void 作为下标 可能会报错
    get_thread_table()[tid] = std::make_tuple(number, _thread, t);		// Override!!!! because tid maybe the same...?
}

void ThreadTable::remove_a_thread(DWORD tid)
{
    LockGuard lg(get_lock());
    get_thread_table().erase(tid);
}

int ThreadTable::get_threadno(DWORD tid)
{
    LockGuard lg(get_lock());
    auto iter = get_thread_table().find(tid);
    if (iter != get_thread_table().end()) {
        return std::get<0>((*iter).second);
    }
    return -1;
}

bool ThreadTable::is_in(DWORD tid)
{
    LockGuard lg(get_lock());
    auto iter = get_thread_table().find(tid);
    if (iter != get_thread_table().end()) {
        return true;
    }
    return false;
}

InstanceOop * ThreadTable::get_a_thread(DWORD tid) {
    LockGuard lg(get_lock());
    auto iter = get_thread_table().find(tid);
    if (iter != get_thread_table().end()) {
        return std::get<1>((*iter).second);
    }
    return nullptr;
}

bool ThreadTable::detect_thread_death(DWORD tid)
{
    //todo: pthread_kill 当参数为0时，是监测线程是否存活,在执行native的时候，由于检查的是本线程，因此肯定是存活的
    //可能这里要考虑其它地方的调用，因此通过 waitfor的方式，立即返回，超时则说明线程正在运行
    HANDLE ret= OpenThread(THREAD_ALL_ACCESS,FALSE,GetCurrentThreadId());
    if (ret!= nullptr) {
        return false;
    }else {
        std::wcerr << "wrong!" << std::endl;		// maybe the EINVAL...
        assert(false);
    }
}

void ThreadTable::print_table()
{
//#ifdef DEBUG
    LockGuard lg(get_lock());
    std::wcout << "===------------- ThreadTable ----------------===" << std::endl;
    for (auto iter : get_thread_table()) {
        std::wcout << "pthread_t :[" << iter.first << "], is the [" << std::get<0>(iter.second) <<
                   "] thread, Thread Oop address: [" << std::dec << std::get<1>(iter.second) << "]"
                                                                                                      ", state:[" << std::get<2>(iter.second)->state << "]" << (!std::get<2>(iter.second)->p.should_be_stop_first ? "(main)" : "")<< std::endl;
    }
    std::wcout << "===------------------------------------------===" << std::endl;
//#endif
}

void ThreadTable::kill_all_except_main_thread(DWORD main_tid)
{
    for (auto iter : get_thread_table()) {
        if (iter.first == main_tid)	continue;
        else {
            //todo:这里相当于触发gc，是本地线程负责执行的，这里没有针对线程发送信号
            if (!raise(SIGINT)) {
                assert(false);
            }
        }
    }
}

void ThreadTable::wake_up_all_thread_force()
{
    signal_all_thread();
}