//
// Created by 14394 on 2020/2/26.
//

#include "../automan_jvm.h"
#include "java_io_UnixFileSystem.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include "native.h"
#include "java_lang_String.h"
#include "../utils/utils.h"
#include <sys/stat.h>

static unordered_map<wstring, void*> methods = {
        {L"initIDs:()V",								(void *)&JVM_UFS_InitIDs},
        {L"canonicalize0:(" STR ")" STR,				(void *)&JVM_Canonicalize0},
        {L"getBooleanAttributes0:(" FLE ")I",			(void *)&JVM_GetBooleanAttributes0},
};

void JVM_UFS_InitIDs(list<Oop *> & _stack){		// static
    // do nothing
}

void JVM_Canonicalize0(list<Oop *> & _stack){
    InstanceOop *_this = (InstanceOop *)_stack.front();	_stack.pop_front();
    InstanceOop *str = (InstanceOop *)_stack.front();	_stack.pop_front();
    std::wstring path = java_lang_string::stringOop_to_wstring(str);
    // in jdk ource code, '-' means `recursive`.
    bool has_final_char = false;
    wchar_t final_char = path[path.size() - 1];
    if (final_char == L'*' || final_char == L'-') {
        path.erase(path.end() - 1);		// remove the final char.
        has_final_char = true;
    }
    //todo: unix文件系统 因为没有引入 boost ,暂时 废弃
    wstring canonical_path = path;
    if (has_final_char) {
        canonical_path += final_char;
    }
    _stack.push_back(java_lang_string::intern(canonical_path));
#ifdef DEBUG
    sync_wcout{} << "(DEBUG) canonical path of [" << java_lang_string::stringOop_to_wstring(str) << "] is [" << java_lang_string::stringOop_to_wstring((InstanceOop *)_stack.back()) << "]." << std::endl;
#endif
}

void JVM_GetBooleanAttributes0(list<Oop *> & _stack){
    InstanceOop *_this = (InstanceOop *)_stack.front();	_stack.pop_front();
    InstanceOop *file = (InstanceOop *)_stack.front();	_stack.pop_front();
    Oop *result;
    file->get_field_value(JFILE L":path:Ljava/lang/String;", &result);
#ifdef DEBUG
    sync_wcout{} << wstring_to_utf8(java_lang_string::stringOop_to_wstring((InstanceOop *)result)).c_str() << std::endl;	// delete
#endif
    std::string backup_str = wstring_to_utf8(java_lang_string::stringOop_to_wstring((InstanceOop *)result));
    const char *path = backup_str.c_str();
    // get file msg
    struct stat64 stat;
    if (stat64(path, &stat) == -1) {
#ifdef DEBUG
        sync_wcout{} << "(DEBUG) didn't find the file: [" << path << "]..." << std::endl;
#endif
        _stack.push_back(new IntOop(0));
        return;
    }
    int mode = 0;
    // Exists:
    mode |= 1;
    // Regular:
    if ((stat.st_mode & S_IFMT) == S_IFREG)	mode |= 2;
    // Dir:
    if ((stat.st_mode & S_IFMT) == S_IFDIR)	mode |= 4;
    _stack.push_back(new IntOop(mode));
#ifdef DEBUG
    sync_wcout{} << "(DEBUG) file: [" << path << "]'s attribute: [" << std::hex << mode << "]." << std::endl;
#endif
}

void *java_io_unixFileSystem_search_method(const wstring & signature)
{
    auto iter = methods.find(signature);
    if (iter != methods.end()) {
        return (*iter).second;
    }
    return nullptr;
}