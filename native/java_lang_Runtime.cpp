//
// Created by 14394 on 2020/2/26.
//

#include "../automan_jvm.h"
#include "java_lang_Runtime.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include "native.h"
#include "../utils/os.h"

static unordered_map<wstring, void*> methods = {
        {L"availableProcessors:()I",				(void *)&JVM_AvailableProcessors},
};

// this method, I simplified it to just run the `run()` method.
void JVM_AvailableProcessors(list<Oop*>& _stack)
{
    _stack.push_back(new IntOop(get_cpu_nums()));
#ifdef DEBUG
    sync_wcout{} << "(DEBUG) now this machine has [" << ((IntOop *)_stack.back())->value << "] cpus." << std::endl;
#endif
}

void *java_lang_runtime_search_method(const wstring & signature)
{
    auto iter = methods.find(signature);
    if (iter != methods.end()) {
        return (*iter).second;
    }
    return nullptr;
}