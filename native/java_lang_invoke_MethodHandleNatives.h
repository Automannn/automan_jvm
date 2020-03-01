//
// Created by 14394 on 2020/2/26.
//

#ifndef AUTOMAN_JVM_JAVA_LANG_INVOKE_METHODHANDLENATIVES_H
#define AUTOMAN_JVM_JAVA_LANG_INVOKE_METHODHANDLENATIVES_H

#include <list>
#include <set>
#include "../utils/lock.h"
#include "../runtime//oop.h"

using std::list;
using std::set;

extern Lock member_name_table_lock;
extern set<InstanceOop *> member_name_table;

InstanceOop* find_table_if_match_methodType(InstanceOop* methodType);

void JVM_GetConstant(list<Oop*>& _stack);
void JVM_Resolve(list<Oop*>& _stack);
void JVM_Expand(list<Oop*>& _stack);
void JVM_Init(list<Oop*>& _stack);
void JVM_MH_ObjjcetFieldOffset(list<Oop*>& _stack);
void JVM_GetMembers(list<Oop*>& _stack);

class vm_thread;

wstring get_member_name_descriptor(InstanceKlass* real_Klass,const wstring& real_name,InstanceOop* type);
Method* get_member_name_target_method(InstanceKlass* real_klass,const wstring& signature,int ref_kind,vm_thread* = nullptr);

void* java_lang_invoke_methodHandleNatives_search_method(const wstring& str);

#endif //AUTOMAN_JVM_JAVA_LANG_INVOKE_METHODHANDLENATIVES_H
