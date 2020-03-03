//
// Created by 14394 on 2020/3/2.
//

#include "sun_io_Win32ErrorMode.h"

static unordered_map<wstring, void*> methods = {
        {L"setErrorMode:(J)J",				(void *)&JVM_SetErrorMode},
};

void JVM_SetErrorMode(list<Oop*>&  _stack){
    //todo: 由于这里不清楚逻辑，返回一个-1即可
    InstanceOop *_this = (InstanceOop *)_stack.front();
    _stack.pop_front();
    long mode = ((LongOop *)_stack.front())->value;	_stack.pop_front();
    //todo: 根据mode本地化设置错误处理方式
    _stack.push_back(new LongOop(-1));

};

void *sun_io_Win32ErrorMode_search_method(const wstring& signature){
    auto iter = methods.find(signature);
    if (iter != methods.end()) {
        return (*iter).second;
    }
    return nullptr;
};