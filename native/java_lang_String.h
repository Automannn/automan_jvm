//
// Created by 14394 on 2020/2/26.
//

#ifndef AUTOMAN_JVM_JAVA_LANG_STRING_H
#define AUTOMAN_JVM_JAVA_LANG_STRING_H

#include <unordered_set>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <list>

#include "../runtime/oop.h"
#include "../utils//lock.h"
#include "../utils/synchronize_wcout.h"

using std::wstring;
using std::shared_ptr;
using std::unordered_set;
using std::wstringstream;
using std::list;

struct java_string_hash{
    size_t operator()(Oop* const& ptr) const noexcept ;
};

struct java_string_equal_to{
    bool operator() (Oop* const& lhs,Oop*const& rhs) const;
};

class java_lang_string{
    friend GC;
private:
    static Lock& getLock(){
        static Lock lock;
        return lock;
    }

    static unordered_set<Oop*,java_string_hash,java_string_equal_to>& get_string_table(){
        static unordered_set<Oop*,java_string_hash,java_string_equal_to> string_table;
        return string_table;
    }

    static Oop* intern_to_oop(const wstring& str);

public:
    static wstring print_stringOop(InstanceOop* stringoop);
    static wstring stringOop_to_wstring(InstanceOop* stringoop);

    static inline  __attribute__((always_inline)) Oop* intern(const wstring& str){
        Oop* stringoop = java_lang_string::intern_to_oop(str);
        LockGuard lg(getLock());
#ifdef STRING_DEBUG
        sync_wcout{} << "===-------------- origin string_table ---------------===" << std::endl;
	for(auto iter : get_string_table()) {
		sync_wcout{} << java_lang_string::print_stringOop((InstanceOop *)iter) << std::endl;
	}
	sync_wcout{} << "===-------------------------------------------===" << std::endl;
#endif
        auto iter = java_lang_string::get_string_table().find(stringoop);
        if (iter == java_lang_string::get_string_table().end()) {
            assert(java_lang_string::get_string_table().find(stringoop) == java_lang_string::get_string_table().end());
            java_lang_string::get_string_table().insert(stringoop);
#ifdef STRING_DEBUG
            sync_wcout{} << java_lang_string::print_stringOop((InstanceOop *)stringoop) << " (insert in)" << std::endl;
#endif
            return stringoop;
    } else{
#ifdef STRING_DEBUG
            sync_wcout{} << java_lang_string::print_stringOop((InstanceOop *)*iter) << " (return directly)" << std::endl;
#endif
            return *iter;
        }
    }
};

void JVM_Intern(list<Oop *> & _stack);

void *java_lang_string_search_method(const wstring & signature);

#endif //AUTOMAN_JVM_JAVA_LANG_STRING_H
