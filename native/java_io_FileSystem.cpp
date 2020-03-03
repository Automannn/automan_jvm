//
// Created by 14394 on 2020/2/26.
//

#include "../automan_jvm.h"
#include "java_io_FileSystem.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include "native.h"
#include "java_lang_String.h"
#include "../utils/utils.h"
#include <sys/stat.h>

static unordered_map<wstring, void*> methods = {
        {L"getLength:(" FLE ")J",					(void *)&JVM_GetLength},
        {L"getBooleanAttributes:(" FLE ")I",			(void *)&JVM_GetBooleanAttributes},
};

void JVM_GetLength(list<Oop *> & _stack){
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
        assert(false);
    }
#ifdef DEBUG
    sync_wcout{} << "(DEBUG) file: [" << path << "]'s length: [" << stat.st_size << "]." << std::endl;
#endif
    _stack.push_back(new LongOop(stat.st_size));
}

void JVM_GetBooleanAttributes(list<Oop *> & _stack){
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

void *java_io_fileSystem_search_method(const wstring & signature)
{
    auto iter = methods.find(signature);
    if (iter != methods.end()) {
        return (*iter).second;
    }
    return nullptr;
}