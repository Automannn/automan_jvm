//
// Created by 14394 on 2020/2/23.
//

#include "../automan_jvm.h"
#include "java_io_FileDescriptor.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include "native.h"

static unordered_map<wstring, void*> methods = {
        {L"initIDs:()V",				(void *)&JVM_FD_InitIDs},
        {L"set:(I)J",				(void *)&JVM_FD_Set},
};

void JVM_FD_InitIDs(list<Oop *> & _stack){		// static
    // do nothing
}

void JVM_FD_Set(list<Oop *>& _stack){
    //todo: 这里是初始化FileDescriptors时，会将标准输入输出，以及错误流 进行与FileOutputStream绑定
    IntOop *fd = (IntOop *)_stack.front();	_stack.pop_front();
    HANDLE ret;
    if(fd->value==0){//标准输入流
        ret= GetStdHandle(STD_INPUT_HANDLE);
    }else if(fd->value==1){//标准输出流
        ret= GetStdHandle(STD_OUTPUT_HANDLE);
    }else{//标准错误流
        ret= GetStdHandle(STD_ERROR_HANDLE);
    }
    long addr = HandleToLong(ret);//ret为指针类型，该指针类型指向long值，*为取出该值
    _stack.push_back(new LongOop(addr));
}

void *java_io_fileDescriptor_search_method(const wstring & signature)
{
    auto iter = methods.find(signature);
    if (iter != methods.end()) {
        return (*iter).second;
    }
    return nullptr;
}