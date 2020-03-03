//
// Created by 14394 on 2020/3/3.
//

#include "java_util_zip_ZipFile.h"

static unordered_map<wstring, void*> methods = {
        //我想估计是设置时间种子之类的作用
        {L"initIDs:()V",				(void *)&JVM_ZIP_initIDs},
};

void JVM_ZIP_initIDs(list<Oop*>& _stack){
    //do nothing
};

void *java_util_zip_ZipFile_search_method(const wstring& signature){
    auto iter = methods.find(signature);
    if (iter != methods.end()) {
        return (*iter).second;
    }
    return nullptr;
};