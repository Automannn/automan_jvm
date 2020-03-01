//
// Created by 14394 on 2020/2/23.
//

#include "constantpool.h"
#include "../native/java_lang_String.h"
#include "../classloader.h"
#include "kclass.h"
#include "field.h"
#include "bytecodeEngine.h"
#include <string>
#include "../utils/synchronize_wcout.h"

using std::make_pair;
using std::wstring;
using std::make_shared;

Klass *rt_constant_pool::if_didnt_load_then_load(ClassLoader *loader, const wstring & name)
{
    if (loader == nullptr) {
        return BootStrapClassLoader::get_bootstrap().loadClass(name);
    } else {
        return loader->loadClass(name);
    }
}

const pair<int, PVOID> & rt_constant_pool::if_didnt_parse_then_parse(int i)
{
    assert(i >= 0 && i < pool.size());
    // 1. if has been parsed, then return
    if (this->pool[i].first != 0)	return this->pool[i];
    // 2. else, parse.
    switch (bufs[i]->tag) {
        case CONSTANT_Class:{
            if (i == this_class_index - 1) {
                this->pool[i] = (make_pair(bufs[i]->tag, (Klass *)this_class));
            } else {
                // get should-be-loaded class name
                CONSTANT_CS_info* target = (CONSTANT_CS_info*)bufs[i];
                assert(bufs[target->index-1]->tag == CONSTANT_Utf8);
                wstring name = ((CONSTANT_Utf8_info *)bufs[target->index-1])->convert_to_Unicode();	// e.g. java/lang/Object
                // load the class
#ifdef DEBUG
                sync_wcout{} << "load class ===> " << "<" << name << ">" << std::endl;
#endif
                Klass *new_class = if_didnt_load_then_load(loader, name);
                assert(new_class != nullptr);
                this->pool[i] = (make_pair(bufs[i]->tag, (Klass *)new_class));
            }
            break;
        }
        case CONSTANT_String:{
            CONSTANT_CS_info* target = (CONSTANT_CS_info*)bufs[i];
            assert(bufs[target->index-1]->tag == CONSTANT_Utf8);
            // make a String Oop...
            wstring* result = (wstring*)((*this)[target->index - 1].second);
            Oop *stringoop = java_lang_string::intern(*result);
            this->pool[i] = (make_pair(bufs[i]->tag, stringoop));
            break;
        }
        case CONSTANT_Fieldref:
        case CONSTANT_Methodref:
        case CONSTANT_InterfaceMethodref:{
            CONSTANT_FMI_info* target = (CONSTANT_FMI_info*)bufs[i];
            assert(bufs[target->class_index-1]->tag == CONSTANT_Class);
            assert(bufs[target->name_and_type_index-1]->tag == CONSTANT_NameAndType);
            // get class name
            wstring class_name = ((CONSTANT_Utf8_info *)bufs[((CONSTANT_CS_info *)bufs[target->class_index-1])->index-1])->convert_to_Unicode();
            // load class
            Klass *new_class = ((Klass *)if_didnt_load_then_load(loader, class_name));
            assert(new_class != nullptr);
            // get field/method/interface_method name
            auto name_type_ptr = (CONSTANT_NameAndType_info *)bufs[target->name_and_type_index-1];
            wstring name = ((CONSTANT_Utf8_info *)bufs[name_type_ptr->name_index-1])->convert_to_Unicode();
            wstring descriptor = ((CONSTANT_Utf8_info *)bufs[name_type_ptr->descriptor_index-1])->convert_to_Unicode();

            if (target->tag == CONSTANT_Fieldref) {
#ifdef DEBUG
                sync_wcout{} << "find field ===> " << "<" << class_name << ">" << name + L":" + descriptor << std::endl;
#endif
                assert(new_class->get_type() == ClassType::InstanceClass);
                Field_info *target = ((InstanceKlass *)new_class)->get_field(name + L":" + descriptor).second;
                assert(target != nullptr);
                this->pool[i] = (make_pair(bufs[i]->tag, target));				// Field_info *
            } else if (target->tag == CONSTANT_Methodref) {
#ifdef DEBUG
                sync_wcout{} << "find class method ===> " << "<" << class_name << ">" << name + L":" + descriptor << std::endl;
#endif
                Method *target;
                // for invoke()!
                wstring real_descriptor;
                if (class_name == L"java/lang/invoke/MethodHandle" &&
                    (name == L"invoke"
                     || name == L"invokeBasic"
                     || name == L"invokeExact"
                     || name == L"invokeWithArauments"
                     || name == L"linkToSpecial"
                     || name == L"linkToStatic"
                     || name == L"linkToVirtual"
                     || name == L"linkToInterface"))
                {
                    real_descriptor = descriptor;		// make a backup
                    descriptor = L"([Ljava/lang/Object;)Ljava/lang/Object;";		// make a substitute...
                }

                if (new_class->get_type() == ClassType::ObjArrayClass || new_class->get_type() == ClassType::TypeArrayClass) {
                    target = ((ArrayKlass *)new_class)->get_class_method(name + L":" + descriptor);
                } else if (new_class->get_type() == ClassType::InstanceClass){
                    target = ((InstanceKlass *)new_class)->get_class_method(name + L":" + descriptor);
                } else {
                    std::cerr << "only support ArrayKlass and InstanceKlass now!" << std::endl;
                    assert(false);
                }
                assert(target != nullptr);
                this->pool[i] = (make_pair(bufs[i]->tag, target));				// Method *

                if (real_descriptor != L"") {	// MethodHandle.invoke(...), the `real_descriptor` now holds the **REAL** descriptor!!
                    target->set_real_descriptor(real_descriptor);
                }

            } else {	// InterfaceMethodref
#ifdef DEBUG
                sync_wcout{} << "find interface method ===> " << "<" << class_name << ">" << name + L":" + descriptor << std::endl;
#endif
                assert(new_class->get_type() == ClassType::InstanceClass);
                Method *target = ((InstanceKlass *)new_class)->get_interface_method(name + L":" + descriptor);
                assert(target != nullptr);
                this->pool[i] = (make_pair(bufs[i]->tag, target));				// Method *
            }
            break;
        }
        case CONSTANT_Integer:{
            CONSTANT_Integer_info* target = (CONSTANT_Integer_info*)bufs[i];
            this->pool[i] = (make_pair(bufs[i]->tag, target->get_value()));		// int
            break;
        }
        case CONSTANT_Float:{
            CONSTANT_Float_info* target = (CONSTANT_Float_info*)bufs[i];
            this->pool[i] = (make_pair(bufs[i]->tag, target->get_value()));		// float
            break;
        }
        case CONSTANT_Long:{
            CONSTANT_Long_info* target = (CONSTANT_Long_info*)bufs[i];
            this->pool[i] = (make_pair(bufs[i]->tag, target->get_value()));		// long
            this->pool[i+1] = (make_pair(-1, nullptr));
            break;
        }
        case CONSTANT_Double:{
            CONSTANT_Double_info* target = (CONSTANT_Double_info*)bufs[i];
            double result = target->get_value();
            this->pool[i] = (make_pair(bufs[i]->tag, target->get_value()));		// double
            this->pool[i+1] = (make_pair(-1, nullptr));
            break;
        }
        case CONSTANT_NameAndType:{
            CONSTANT_NameAndType_info* target = (CONSTANT_NameAndType_info*)bufs[i];
            this->pool[i] = (make_pair(bufs[i]->tag, make_pair((int)target->name_index, (int)target->descriptor_index)));	// pair<int, int> 。
            break;
        }
        case CONSTANT_Utf8:{
            CONSTANT_Utf8_info* target = (CONSTANT_Utf8_info*)bufs[i];
            this->pool[i] = (make_pair(bufs[i]->tag, target->convert_to_Unicode()));		// wstring。
            break;
        }
        case CONSTANT_MethodHandle:{
            CONSTANT_MethodHandle_info *target = (CONSTANT_MethodHandle_info*)bufs[i];
            this->pool[i] = (make_pair(bufs[i]->tag, make_pair((int)target->reference_kind, (int)target->reference_index)));	// pair<int, int> 。
            break;
        }
        case CONSTANT_MethodType:{
            CONSTANT_MethodType_info *target = (CONSTANT_MethodType_info*)bufs[i];
            assert(bufs[target->descriptor_index-1]->tag == CONSTANT_Utf8);
            wstring* result = (wstring*)((*this)[target->descriptor_index - 1].second);
            this->pool[i] = (make_pair(bufs[i]->tag, result));
            break;
        }
        case CONSTANT_InvokeDynamic:{
            CONSTANT_InvokeDynamic_info *target = (CONSTANT_InvokeDynamic_info*)bufs[i];
            //todo: make_pair 返回的 是 pair,而需要的泛型类型是： pvoid ，编译可能会出错
            this->pool[i] = (make_pair(bufs[i]->tag, make_pair((int)target->bootstrap_method_attr_index, (int)target->name_and_type_index)));	// pair<int, int> 。
            break;
        }
        default:{
            std::cerr << "wrong! tag == " << (int)bufs[i]->tag << std::endl;
            std::cerr << "didn't finish MethodHandle, MethodType and InvokeDynamic!!" << std::endl;
            assert(false);
        }
    }
    return this->pool[i];
}