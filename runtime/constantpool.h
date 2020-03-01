//
// Created by 14394 on 2020/2/23.
//

#ifndef AUTOMAN_JVM_CONSTANTPOOL_H
#define AUTOMAN_JVM_CONSTANTPOOL_H

#include "../class_parser.h"
#include <vector>
#include <utility>
#include <cassert>
#include <memory>
#include <windows.h>

using std::vector;
using std::pair;
using std::wstring;
using std::shared_ptr;

class Klass;
class InstanceKlass;
class GC;

class rt_constant_pool{
    friend GC;
private:
    InstanceKlass* this_class;
    int this_class_index;
    cp_info** bufs;
    vector<pair<int,PVOID>> pool;
    ClassLoader* loader;
private:
    Klass* if_didnt_load_then_load(ClassLoader* loader, const wstring& name);

public:
    explicit rt_constant_pool(InstanceKlass* this_class,ClassLoader* loader,ClassFile* cf):this_class(this_class),loader(loader),
    this_class_index(cf->this_class),bufs(cf->constant_pool),pool(cf->constant_pool_count-1,std::make_pair(0, nullptr)){}

private:
    const pair<int,PVOID>& if_didnt_parse_then_parse(int index);

public:
    pair<int,PVOID> operator[] (int index){
        return if_didnt_parse_then_parse(index);
    }

public:
    void print_debug();
};


class constantpool {

};


#endif //AUTOMAN_JVM_CONSTANTPOOL_H
