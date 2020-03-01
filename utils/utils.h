//
// Created by 14394 on 2020/2/23.
//

#ifndef AUTOMAN_JVM_UTILS_H
#define AUTOMAN_JVM_UTILS_H

#include <string>
#include <locale>
#include <codecvt>
#include <typeinfo>
#include <iostream>
#include <vector>

using std::wstring;
using std::vector;

//convert wstring to UTF-8 string
std::string  wstring_to_utf8(const std::wstring& str);

//convert UTF-8 to wstring
std::wstring utf8_to_wstring(const std::string& str);

//yet another convert wstring to UTF-8 string
//std::string narrow(const std::wstring& str);

//parse field descriptor to get length (or type)
//int parse_field_descriptor(const std::wstring& descriptor);

extern vector<wstring>& split(wstring str,wstring splitter);

/*******************debugging tool******************************************/
class vm_thread;
class InstanceOop;

//std::wstring toString(InstanceOop *oop,vm_thread *thread);

/********************constructor*********************************************/
//constructors and destructors
template <typename  Tp,typename Arg1>
void __constructor(Tp*ptr,const Arg1& arg1){
    ::new((void *)ptr) Tp(arg1);
}

template <typename Tp,typename Arg1,typename Arg2>
void __constructor(Tp* ptr,const Arg1& arg1,const Arg2& arg2){
    ::new((void *)ptr) Tp(arg1,arg2);
}

template <typename Tp,typename Arg1,typename Arg2>
void __constructor(Tp* ptr,const Arg1& arg1,Arg2& arg2){
    ::new((void *)ptr) Tp(arg1,arg2);
}

template <typename Tp,typename Arg1,typename Arg2,typename Arg3>
void __constructor(Tp* ptr,const Arg1& arg1,const Arg2& arg2,const Arg3& arg3){
    ::new((void *)ptr) Tp(arg1,arg2,arg3);
}

template <typename Tp,typename  ...Args>
void constructor(Tp* ptr,Args&& ...args){
    __constructor(ptr,std::forward<Args>(args)...);
}

template <typename Tp>
void destructor(Tp* ptr){
    if(ptr)
        ptr->~Tp();
}



#endif //AUTOMAN_JVM_UTILS_H
