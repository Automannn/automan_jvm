//
// Created by 14394 on 2020/2/23.
//

#ifndef AUTOMAN_JVM_KCLASS_H
#define AUTOMAN_JVM_KCLASS_H

#include <unordered_map>
#include <vector>
#include <utility>
#include <list>
#include "method.h"
#include "../class_parser.h"
#include "../utils/synchronize_wcout.h"
#include "../utils/lock.h"

using std::unordered_map;
using std::vector;
using std::pair;
using std::list;

enum Type{
    _BYTE,
    _BOOLEAN,
    _CHAR,
    _SHORT,
    _INT,
    _FLOAT,
    _LONG,
    _DOUBLE,
    _OBJECT,
    _ARRAY,
    _VOID,
};

enum ClassType{
    InstanceClass,
    ObjArrayClass,
    TypeArrayClass,
};

enum OopType{
    _InstanceOop,
    _BasicTypeOop,
    _ObjArrayOop,
    _TypeArrayOop,
};

class Oop;
class MirrorOop;
class rt_constant_pool;

Type get_type(const wstring& name);

class Rt_Pool{
private:
    static Lock& rt_pool_lock();

private:
    static list<rt_constant_pool*>& rt_pool();

public:
    static void put(rt_constant_pool* pool);
    static void cleanup();
};

class GC;

class Klass{
    friend GC;
public:
    enum KlassState{NotInitialized,Initializing,Initialized};
protected:
    KlassState state = KlassState::NotInitialized;
protected:
    ClassType classType;
    wstring  name;
    u2 access_flags;

    MirrorOop* java_mirror = nullptr;
    Klass* parent = nullptr;

public:
    KlassState get_state(){ return state;}
    void set_state(KlassState s){state = s;}

    Klass* get_parent(){return parent;}
    void set_parent(Klass* parent){this->parent = parent;}

    int get_access_flags(){return access_flags;}
    void set_access_flags(int access_flags){this->access_flags = access_flags;}

    wstring get_name(){return this->name;}
    void set_name(const wstring& name){this->name = name;}

    ClassType get_type(){return classType;}
    MirrorOop* get_mirror(){return java_mirror;}
    void set_mirror(MirrorOop* mirror){java_mirror = mirror;}

public:
    bool is_interface(){
        return (this->access_flags& ACC_INTERFACE)==ACC_INTERFACE;
    }

protected:
    Klass(const Klass&);
    Klass operator=(const Klass &);

public:
    Klass(){}
    virtual ~Klass(){}
};

class Field_info;
class rt_constant_pool;
class InstanceOop;

class InstanceKlass: public Klass{
    friend InstanceOop;
    friend MirrorOop;
    friend GC;

private:
    ClassLoader* loader;
    MirrorOop* java_loader = nullptr;

    u2 constant_pool_count;
    cp_info** constant_pool;

    unordered_map<wstring,InstanceKlass*> interfaces;
    unordered_map<int,bool> is_this_klass_field;
    unordered_map<wstring,pair<int,Field_info*>> fields_layout;
    unordered_map<wstring,pair<int,Field_info*>> static_fields_layout;

    int total_non_static_fields_num = 0;
    int total_static_fields_num = 0;

    vector<Oop*> static_fields;
    unordered_map<wstring ,Method*> vtable;

    unordered_map<wstring,pair<int,Method*>> methods;

    rt_constant_pool* rt_pool;

    u2 attributes_count;
    attribute_info** attributes;

    InnerClasses_attribute* inner_classes = nullptr;
    EnclosingMethod_attribute* enclosing_method = nullptr;

    wstring sourceFile;
    u2 signature_index = 0;
    BootstrapMethods_attribute* bm = nullptr;

    Parameter_annotations_t* rva = nullptr;
    u2 num_RuntimeVisibleTypeAnnotations = 0;
    TypeAnnotation* rvta = nullptr;

    InstanceKlass* host_klass = nullptr;

private:
    void parse_method(ClassFile* cf);
    void parse_fields(ClassFile* cf);
    void parse_superclass(ClassFile* cf,ClassLoader* loader);
    void parse_interfaces(ClassFile* cf,ClassLoader* loader);
    void parse_attributes(ClassFile* cf);

public:
    void parse_constantpool(ClassFile* cf,ClassLoader* loader);

public:
    Method* get_static_void_main();
    void initialize_final_static_field();
    wstring parse_signature();
    wstring get_source_file_name();
    PVOID get_enclosing_method(){return enclosing_method;}
    PVOID get_inner_class(){return inner_classes;}

public:
    vector<Oop*>& get_static_fields_addr(){return static_fields;}

private:
    void initialize_field(unordered_map<wstring,pair<int,Field_info*>>& fields_layout,vector<Oop*>& fields);

public:
    InstanceKlass* get_hostklass(){ return host_klass;}
    void set_hostklass(InstanceKlass* hostklass){host_klass = hostklass;}

    MirrorOop* get_java_loader(){return this->java_loader;}

    pair<int,Field_info*> get_field(const wstring& descriptor);
    Method* get_class_method(const wstring& signature, bool search_interfaces = true);
    Method* get_this_class_method(const wstring& signature);
    Method* get_interface_method(const wstring& signature);

    int non_static_field_num(){return total_non_static_fields_num;}
    bool get_static_field_value(Field_info *field,Oop** result);
    void set_static_field_value(Field_info *field,Oop* result);

    bool get_static_field_value(const wstring& signature,Oop** result);
    void set_static_field_value(const wstring& signature,Oop* value);

    Method* search_vtable(const wstring& signature);
    rt_constant_pool* get_rtpool(){return rt_pool;}
    ClassLoader* get_classloader(){return this->loader;}

    bool check_interfaces(const wstring& signature);
    bool check_interfaces(InstanceKlass* klass);
    bool check_parent(const wstring& signature){
        if(parent== nullptr) return false;
        bool success1 = parent->get_name() == signature;
        bool success2 = (parent== nullptr)?false:((InstanceKlass*)parent)->check_parent(signature);
        return success1 || success2;
    }

    bool check_parent(InstanceKlass* klass){
        if(parent== nullptr) return  false;
        bool success1 = parent==klass;
        bool success2 = (parent== nullptr)? false:((InstanceKlass*)parent)->check_parent(klass);
        return success1||success2;
    }

    InstanceOop* new_instance();
    Method* search_method_in_slot(int slot);

private:
    int get_static_field_offset(const wstring& signature);

public:
    int get_all_field_offset(const wstring& BIG_signature);

public:
    bool is_interface(){
        return (this->access_flags& ACC_INTERFACE) == ACC_INTERFACE;
    }

public:
    vector<pair<int,Method*>> get_constructors();
    vector<pair<int,Method*>> get_declared_methods();

public:
    bool is_in_vtable(Method* m){
        wstring signature = m->get_name()+L":"+m->get_descriptor();
        return vtable.find(signature)!=vtable.end();
    }

    const unordered_map<wstring,pair<int,Field_info*>>&  get_field_layout(){
        return this->fields_layout;
    }

    const unordered_map<wstring,pair<int,Field_info*>>&  get_static_field_layout(){
        return this->static_fields_layout;
    }

    BootstrapMethods_attribute* get_bm(){
        return this->bm;
    }

private:
    InstanceKlass(const InstanceKlass&);

public:
    InstanceKlass(ClassFile* cf,ClassLoader* loader,MirrorOop* java_loader= nullptr,ClassType type = ClassType::InstanceClass);
    ~InstanceKlass();
};

class MirrorOop;
class ArrayOop;

class MirrorKlass: public InstanceKlass{
private:
    MirrorKlass();

public:
    MirrorOop* new_mirror(Klass* mirrored_who,MirrorOop* loader);
};

class ArrayKlass:public Klass{
    friend GC;
private:
    ClassLoader* loader;
    MirrorOop* java_loader = nullptr;

    int dimension;
    Klass* higher_dimension = nullptr;
    Klass* lower_dimension = nullptr;

public:
    Klass* get_higher_dimension(){
        return higher_dimension;
    }
    void set_higher_dimension(Klass* higher){
        higher_dimension = higher;
    }

    Klass* get_lower_dimension(){
        return lower_dimension;
    }
    void set_lower_dimension(Klass* lower){
        lower_dimension = lower;
    }

    int get_dimension(){
        return dimension;
    }

    ArrayOop* new_instance(int lenght);

private:
    ArrayKlass(const ArrayKlass&);

public:
    MirrorOop* get_java_loader(){
        return this->java_loader;
    }

    Method* get_class_method(const wstring& signature){
        Method* target = ((InstanceKlass*)parent)->get_class_method(signature);
        assert(target!= nullptr);
        return target;
    }

public:
    ArrayKlass(int dimension,ClassLoader* loader,Klass* lower_dimension,Klass* higher_dimension,MirrorOop* java_loader,ClassType classtype);
    ~ArrayKlass(){};
};

class TypeArrayKlass: public ArrayKlass{
private:
    Type type;
public:
    Type get_basic_type(){
        return type;
    }

private:
    TypeArrayKlass(const TypeArrayKlass&);

public:
    TypeArrayKlass(Type type,int dimension,ClassLoader* loader,Klass* lower_dimension,Klass* higher_dimension,MirrorOop* java_loader= nullptr,ClassType classtype=ClassType::TypeArrayClass);
    ~TypeArrayKlass(){};
};

class ObjArrayKlass: public ArrayKlass{
private:
    InstanceKlass* element_klass = nullptr;
public:
    InstanceKlass* get_element_klass(){return element_klass;}

private:
    ObjArrayKlass(const ObjArrayKlass&);

public:
    ObjArrayKlass(InstanceKlass* element,int dimension,ClassLoader* loader,Klass* lower_dimension,Klass* higher_dimension,MirrorOop* java_loader = nullptr,ClassType classtype=ClassType::ObjArrayClass);
    ~ObjArrayKlass(){};
};



#endif //AUTOMAN_JVM_KCLASS_H
