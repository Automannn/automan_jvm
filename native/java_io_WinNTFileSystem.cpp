//
// Created by 14394 on 2020/3/2.
//

#include "../automan_jvm.h"
#include "java_io_WinNTFileSystem.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include "native.h"
#include "java_lang_String.h"
#include "../utils/utils.h"
#include <sys/stat.h>

static unordered_map<wstring, void*> methods = {
        {L"initIDs:()V",				(void *)&JVM_InitIDs},
        {L"canonicalize0:(" STR ")" STR,				(void *)&JVM_WIN_Canonicalize0},
};

void JVM_InitIDs(list<Oop*>& _stack){
    //todo: 由于这里不清楚逻辑，暂不做操作
    InstanceOop *_this = (InstanceOop *)_stack.front();
};

void JVM_WIN_Canonicalize0(list<Oop*>& _stack){
    InstanceOop *_this = (InstanceOop *)_stack.front();	_stack.pop_front();
    InstanceOop *str = (InstanceOop *)_stack.front();	_stack.pop_front();

    std::wstring path = java_lang_string::stringOop_to_wstring(str);

    // in jdk source code, '-' means `recursive`.
    bool has_final_char = false;
    wchar_t final_char = path[path.size() - 1];
    if (final_char == L'*' || final_char == L'-') {
        path.erase(path.end() - 1);		// remove the final char.
        has_final_char = true;
    }
    //todo: 由于没有使用boost，因此 暂时不做规范化，直接返回
    wstring canonical_path = path;
    std::wcout<<L"canonical_path is "<<path<<std::endl;

    if (has_final_char) {
        canonical_path += final_char;
    }

    _stack.push_back(java_lang_string::intern(canonical_path));

#ifdef DEBUG
    sync_wcout{} << "(DEBUG) canonical path of [" << java_lang_string::stringOop_to_wstring(str) << "] is [" << java_lang_string::stringOop_to_wstring((InstanceOop *)_stack.back()) << "]." << std::endl;
#endif
}



void *java_io_WinNTFileSystem_search_method(const wstring& signature){
    auto iter = methods.find(signature);
    if (iter != methods.end()) {
        return (*iter).second;
    }
    return nullptr;
}
