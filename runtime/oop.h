//
// Created by 14394 on 2020/2/23.
//

#ifndef AUTOMAN_JVM_OOP_H
#define AUTOMAN_JVM_OOP_H

#include "../runtime/kclass.h"
#include "../runtime//field.h"
#include "../utils/monitor.h"
#include "../utils/lock.h"

#include <cstdlib>
#include <cstring>
#include <memory>
#include <list>

using std::shared_ptr;
using std::list;

class Mempool{
public:
    static list<Oop *>& oop_handler_pool(){
        static list<Oop*> oop_handler_pool;
        return oop_handler_pool;
    }

    static Oop* copy(Oop& oop);
};

class MemAlloc{
private:
    static Lock& mem_lock(){
        static Lock mem_lock;
        return mem_lock;
    }

public:
    static void* allocate(size_t size,bool);
    static void deallocate(void* ptr);
    static void* operator new(size_t size,bool = false) throw();
    static void* operator new(size_t size,const std::nothrow_t&) throw(){exit(-2);} //do not use it
    static void* operator new[](size_t size,bool = false) throw();
    static void* operator new[](size_t size,const std::nothrow_t&) throw(){exit(-2);} //do not use it

    static void operator delete(void* ptr);
    static void operator delete[](void* ptr);

    static void cleanup();
};

class GC;

class Oop:public MemAlloc{
    friend GC;

protected:
    // hashcode .etc
    OopType oopType;
    Klass* klass = nullptr;
    Monitor m;

public:
    Klass* get_klass(){return klass;}
    OopType get_ooptype(){return oopType;}

public:
    void enter_monitor(){m.enter();}
    void wait(){m.wait();}
    void wait(long macro_sec){m.wait(macro_sec);}
    void notify(){m.notify();}
    void notify_all(){m.notify_all();}
    void leave_monitor(){m.leave();}
    void force_unlock_when_athrow(){m.force_unlock_when_athrow();}

public:
    explicit Oop(Klass* klass,OopType oopType):klass(klass),oopType(oopType){}
    Oop(const Oop& rhs):oopType(rhs.oopType),klass(rhs.klass){}

    virtual ~Oop(){}
    virtual Oop* copy();
};

class InstanceOop:public Oop{
    friend GC;
private:
    int field_length;
    vector<Oop*> fields;
public:
    InstanceOop(InstanceKlass* klass);
    InstanceOop(const InstanceOop& rhs);

public:
    bool get_field_value(Field_info* field,Oop** result);
    void set_field_value(Field_info* field,Oop* value);
    bool get_field_value(const wstring& BIG_signature,Oop** result);
    void set_field_value(const wstring& BIG_signature,Oop* value);
    bool get_static_field_value(Field_info* field,Oop** result){return ((InstanceKlass*)klass)->get_static_field_value(field,result);}
    void set_static_field_value(Field_info *field, Oop* value) { ((InstanceKlass*)klass)->set_static_field_value(field,value); }
    bool get_static_field_value(const wstring & signature, Oop **result) { return ((InstanceKlass *)klass)->get_static_field_value(signature, result); }
    void set_static_field_value(const wstring & signature, Oop *value) { ((InstanceKlass *)klass)->set_static_field_value(signature, value); }

public:
    int get_all_field_offset(const wstring& BIG_signature);
    vector<Oop*>& get_fields_addr(){return fields;}

private:
    int get_static_field_offset(const wstring& signature);

public:
    virtual Oop* copy();
};

class MirrorOop: public InstanceOop{
    friend GC;
private:
    Klass* mirrored_who = nullptr;
    wstring extra;

public:
    wstring get_extra(){return extra;}
    void set_extra(const wstring& s){extra = s;}

public:
    MirrorOop(Klass* mirrored_who);
    MirrorOop(const MirrorOop& rhs);

public:
    Klass* get_mirrored_who(){return mirrored_who;}
    void set_mirrored_who(Klass* mirrored_who){this->mirrored_who = mirrored_who;}

    const unordered_map<wstring,pair<int,Field_info*>>& get_mirrored_all_fields(){return((InstanceKlass*)mirrored_who)->fields_layout;}
    const unordered_map<wstring,pair<int,Field_info*>>& get_mirrored_all_static_fields(){return((InstanceKlass*)mirrored_who)->static_fields_layout;}

    bool is_the_field_owned_by_this(int offset){
        auto& this_fields_map = ((InstanceKlass*)mirrored_who)->is_this_klass_field;
        auto iter = this_fields_map.find(offset);
        assert(iter!=this_fields_map.end());
        return iter->second;
    }

public:
    virtual Oop* copy();
};

class ArrayOop: public Oop{
    friend GC;
protected:
    vector<Oop*> buf;

public:
    ArrayOop(ArrayKlass* klass,int length,OopType ooptype):Oop(klass,ooptype){
        buf.resize(length);
    }
    ArrayOop(const ArrayOop & rhs);
    int get_length() { return buf.size(); }
    int get_dimension() { return ((ArrayKlass *)klass)->get_dimension(); }

    Oop* & operator[] (int index) {
        assert(index >= 0 && index < buf.size());	// please replace with ArrayIndexOutofBound...
        return buf[index];
    }

    const Oop* operator[] (int index) const {
        return this->operator[](index);
    }
    int get_buf_offset() {		// use for sun/misc/Unsafe...
//		return ((char *)&buf - (char *)this);
        return 0;
    }
public:		// for gc:
    virtual Oop* copy();
};


class TypeArrayOop : public ArrayOop {
public:		// Most inner type of `buf` is BasicTypeOop.
    TypeArrayOop(TypeArrayKlass *klass, int length) : ArrayOop(klass, length, OopType::_TypeArrayOop) {}
public:

};

class ObjArrayOop : public ArrayOop {
public:		// Most inner type of `buf` is InstanceOop.
    ObjArrayOop(ObjArrayKlass * klass, int length) : ArrayOop(klass, length, OopType::_ObjArrayOop) {}
public:
};

class BasicTypeOop : public Oop {
private:
    Type type;	// only allow for CHAR, INT, FLOAT, LONG, DOUBLE.		[BYTE, BOOLEAN and SHORT are all INT!!!]
public:
    BasicTypeOop(Type type) : Oop(nullptr, OopType::_BasicTypeOop), type(type) {}
    Type get_type() { return type; }
    virtual Oop *copy() override;
};

struct IntOop : public BasicTypeOop {
    int value;		// data
    IntOop(int value) : BasicTypeOop(Type::_INT), value(value) {}
    virtual Oop *copy() override;
};

struct FloatOop : public BasicTypeOop {
    float value;		// data
    FloatOop(float value) : BasicTypeOop(Type::_FLOAT), value(value) {}
    virtual Oop *copy() override;
};

struct LongOop : public BasicTypeOop {
    long value;		// data
    LongOop(long value) : BasicTypeOop(Type::_LONG), value(value) {}
    virtual Oop *copy() override;
};

struct DoubleOop : public BasicTypeOop {
    double value;		// data
    DoubleOop(float value) : BasicTypeOop(Type::_DOUBLE), value(value) {}
    virtual Oop *copy() override;
};


#endif //AUTOMAN_JVM_OOP_H
