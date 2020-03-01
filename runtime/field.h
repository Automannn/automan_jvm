//
// Created by 14394 on 2020/2/23.
//

#ifndef AUTOMAN_JVM_FIELD_H
#define AUTOMAN_JVM_FIELD_H

#include <vector>
#include <string>
#include <cassert>
#include <memory>
#include <list>

#include "../utils/utils.h"
#include "../class_parser.h"
#include "../runtime/kclass.h"
#include "annotation.h"
#include "../utils/lock.h"

using std::wstring;
using std::vector;

class InstanceKlass;
class Field_info;

class Field_Pool{
private:
    static Lock& field_pool_lock();

private:
    static list<Field_info*>& field_pool();

public:
    static void put(Field_info* field);
    static void cleanup();
};

class Field_info{
public:
    enum FieldState{NotParsed,Parsing,Parsed};
private:
    FieldState state = NotParsed;

    InstanceKlass* klass = nullptr;

    Type type;

    Klass* true_type = nullptr;

    u2 access_flags;
    wstring name;
    wstring descriptor;

    u2 attributes_count;
    attribute_info** attributes;

    Parameter_annotations_t* rva = nullptr;

    u2 num_RuntimeVisibleTypeAnnotations = 0;
    TypeAnnotation* rvta = nullptr;

    ConstantValue_attribute* constant_value = nullptr;
    u2 signature_index=0;

private:
    void if_didnt_parse_then_parse();

public:
    explicit Field_info(InstanceKlass* klass,field_info& fi,cp_info** constant_pool);
    FieldState get_state(){return state;}
    void set_state(FieldState s){state = s;}

    const wstring& get_name(){return name;}
    const wstring& get_descriptor(){return descriptor;}

    InstanceKlass* get_klass(){return klass;}
    Type get_type(){if_didnt_parse_then_parse();return type;}

    Klass* get_type_klass(){if_didnt_parse_then_parse(); return true_type;}
    u2 get_flag(){return access_flags;}

    CodeStub* get_rva(){if(rva) return &rva->stub; else return nullptr;}

public:
    bool is_public(){return (access_flags&ACC_PUBLIC)==ACC_PUBLIC;}
    bool is_static(){return (access_flags&ACC_STATIC)==ACC_STATIC;}
    bool is_final(){return (access_flags&ACC_FINAL)==ACC_FINAL;}
    bool is_volatile(){return (access_flags&ACC_VOLATILE)==ACC_VOLATILE;}

public:
    ConstantValue_attribute* get_constant_value(){return constant_value;}
    wstring parse_signature();

public:
    void print(){
#ifdef DEBUG
        sync_wcout{} << name << ":" << descriptor;
#endif
    }

    ~Field_info();

};


#endif //AUTOMAN_JVM_FIELD_H
