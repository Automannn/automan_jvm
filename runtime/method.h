//
// Created by 14394 on 2020/2/23.
//

#ifndef AUTOMAN_JVM_METHOD_H
#define AUTOMAN_JVM_METHOD_H

#include <string>
#include <cassert>
#include <memory>
#include <unordered_map>
#include "list"
#include "constantpool.h"
#include "../class_parser.h"
#include "annotation.h"
#include "../utils/synchronize_wcout.h"
#include "../utils/lock.h"

using std::wstring;
using std::unordered_map;
using std::list;

class MirrorOop;
class InstanceKlass;
class Method;

class MethodPool{
private:
    static Lock& method_pool_lock();

private:
    static list<Method*>& method_pool();

public:
    static void put(Method* method);
    static void cleanup();
};

/**
 *  single Method. save in InstanceKlass
 * */
class Method{
private:
    InstanceKlass* klass = nullptr;
    wstring  name;
    wstring descriptor;

    wstring real_descriptor;
    u2 access_flags;

    u2 attributes_count;
    attribute_info** attributes;

    Parameter_annotations_t* rva = nullptr;
    u1 num_RuntimeVisibleParameterAnnotation=0;
    Parameter_annotations_t* rvpa = nullptr;

    CodeStub _rvpa;

    u2 num_RuntimeVisibleTypeAnnotations = 0;
    TypeAnnotation* rvta = nullptr;

    Code_attribute* code = nullptr;
    LineNumberTable_attribute *lnt = nullptr;

    u2 Code_num_RuntimeVisibleTypeAnnotations =0;
    TypeAnnotation* code_rvta = nullptr;

    Exceptions_attribute* exceptions = nullptr;
    bool parsed = false;
    unordered_map<wstring,Klass *> exceptions_tb;

    u2 signature_index = 0;
    Element_value* ad = nullptr;
    CodeStub _ad;

public:
    bool is_static() { return (this->access_flags & ACC_STATIC) == ACC_STATIC; }
    bool is_public() { return (this->access_flags & ACC_PUBLIC) == ACC_PUBLIC; }
    bool is_private() { return (this->access_flags & ACC_PRIVATE) == ACC_PRIVATE; }
    bool is_void() { return descriptor[descriptor.size()-1] == L'V'; }
    bool is_return_primitive() { return descriptor[descriptor.size()-1] != L';'; }
    bool is_main() { return is_static() && is_public() && is_void() && name == L"main" && descriptor == L"([Ljava/lang/String;)V"; }
    bool is_native() { return (this->access_flags & ACC_NATIVE) == ACC_NATIVE; }
    bool is_abstract() { return (this->access_flags & ACC_ABSTRACT) == ACC_ABSTRACT; }
    bool is_synchronized() { return (this->access_flags & ACC_SYNCHRONIZED) == ACC_SYNCHRONIZED; }
    wstring return_type() { return return_type(this->descriptor); }
    u2 get_flag() { return access_flags; }

public:
    static wstring return_type(const wstring & descriptor) { return descriptor.substr(descriptor.find_first_of(L")")+1); }
    static vector<MirrorOop *> parse_argument_list(const wstring & descriptor);
    static MirrorOop *parse_return_type(const wstring & return_type);

public:
    void set_real_descriptor (const wstring & real_descriptor) { this->real_descriptor = real_descriptor; }	// for MethodHandle.invoke**(...) only.
    vector<MirrorOop *> if_didnt_parse_exceptions_then_parse();
    vector<MirrorOop *> parse_argument_list();
    MirrorOop *parse_return_type();
    int get_java_source_lineno(int pc_no);

public:
    bool has_annotation_name_in_method(const wstring & name) {
        if (rvpa != nullptr && rvpa->has_annotation_name(name)) return true;
        if (rva != nullptr && rva->has_annotation_name(name))	return true;
        if (rvta != nullptr && rvta->has_annotation_name(name))	return true;
        return false;
    }
    void print_all_attribute_name() {		// for debug
#ifdef DEBUG
        sync_wcout{} << "===-------------- print attributes name -------------------===" << std::endl;
		sync_wcout{} << "--- Runtime Visible Annotations: \n";
		if (rva != nullptr)
			for (int i = 0; i < rva->num_annotations; i ++) {
				sync_wcout{} << rva->annotations[i].type << std::endl;
			}
		else sync_wcout{} << "none." << std::endl;
		sync_wcout{} << "--- Runtime Visible Parameter Annotations: \n";
		if (rvpa != nullptr)
			for (int i = 0; i < num_RuntimeVisibleParameterAnnotation; i ++) {
				for (int j = 0; j < rvpa[i].num_annotations; j ++) {
					sync_wcout{} << rvpa[i].annotations[j].type << std::endl;
				}
			}
		else sync_wcout{} << "none." << std::endl;
		sync_wcout{} << "--- Runtime Visible Type Annotations: \n";
		if (rvta != nullptr)
			for (int i = 0; i < num_RuntimeVisibleTypeAnnotations; i ++) {
				sync_wcout{} << rvta[i].anno->type << std::endl;
			}
		else sync_wcout{} << "none." << std::endl;
		sync_wcout{} << "===--------------------------------------------------------===" << std::endl;
#endif
    }

public:
    bool operator== (const Method & rhs) const {
        return this->name == rhs.name && this->descriptor == rhs.descriptor;
    }

public:
    Method(InstanceKlass *klass, method_info & mi, cp_info **constant_pool);
    const wstring & get_name() { return name; }
    const wstring & get_descriptor() { return descriptor; }
    const Code_attribute *get_code() { return code; }
    InstanceKlass *get_klass() { return klass; }
    wstring parse_signature();
    void print() { sync_wcout{} << name << ":" << descriptor; }
    CodeStub *get_rva() { if (rva) return &rva->stub; else return nullptr;}
    CodeStub *get_rvpa() { if (rvpa) return &this->_rvpa; else return nullptr;}
    CodeStub *get_ad() { if (ad) return &this->_ad; else return nullptr;}
    int where_is_catch(int cur_pc, InstanceKlass *cur_excp);

    ~Method();
};


#endif //AUTOMAN_JVM_METHOD_H
