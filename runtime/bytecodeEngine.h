//
// Created by 14394 on 2020/2/23.
//

#ifndef AUTOMAN_JVM_BYTECODEENGINE_H
#define AUTOMAN_JVM_BYTECODEENGINE_H

#include <vector>
#include <stack>
#include <memory>
#include <string>
#include <list>

#include "oop.h"

using std::list;
using std::wstring;
using std::vector;
using std::stack;
using std::shared_ptr;

#define T_BOOLEAN 4
#define T_CHAR 5
#define T_FLOAT	6
#define T_DOUBLE 7
#define T_BYTE 8
#define T_SHORT 9
#define T_INT 10
#define T_LONG 11

class Method;
class vm_thread;

struct StackFrame{
public:
    bool valid_frame = true;
    stack<Oop*> op_stack;
    vector<Oop*> localVariableTable;
    Method* method;
    uint8_t * return_pc;
    StackFrame* prev;
    bool has_exception = false;

public:
    StackFrame(Method* method,uint8_t* return_pc,StackFrame* prev,const list<Oop*>& list,vm_thread* thread,bool is_native=false);
    bool is_valid(){return valid_frame;}
    void set_invalid(){valid_frame=false;}

    void clear_all();
    void reset_method(Method* new_method){this->method = new_method;}
    void reset_return_pc(uint8_t* return_pc){this->return_pc = return_pc;}

    wstring  print_arg_msg(Oop* value,vm_thread* thread);
};

class vm_thread;

struct DebugTool{
    static bool& is_open();
    static unordered_map<wstring,pair<wstring,wstring>>& get_config_map();
    static bool match(const wstring& name,const wstring& descriptor,const wstring& klass_name){
        auto& map = get_config_map();
        auto iter = map.find(name);
        if(iter==map.end()){
            return false;
        } else{
            if(iter->second.first==descriptor&&iter->second.second == klass_name){
                return true;
            }
        }
        return false;
    }
};

struct BytecodeEngine{
public:
    static Oop* execute(vm_thread& thread,StackFrame& cur_frame,int thread_no);

public:
    static vector<wstring> parse_arg_list(const wstring& descriptor);
    static void initial_client(InstanceKlass*,vm_thread& thread);

    static bool check_instanceof(Klass* ref_klass,Klass* klass);
    static wstring get_real_value(Oop* oop);

    static void main_thread_exception(int exitcode=-1);

private:
    static InstanceOop* MethodHandle_make(rt_constant_pool& rt_pool,int method_handle_real_index,vm_thread& thread,bool = false);
    static InstanceOop* MethodType_make(Method* target_method,vm_thread& thread);
    static InstanceOop* MethodType_make(const wstring& descriptor,vm_thread& thread);

    static InstanceOop* MethodType_make_impl(vector<MirrorOop*>& args,MirrorOop* ret,vm_thread& thread);
    static InstanceOop* MethodHandles_Lookup_make(vm_thread& thread);

private:
    static void getField(Field_info* new_field,stack<Oop*>& op_stack);
    static void getStatic(Field_info* new_field,stack<Oop*>& op_stack,vm_thread& thread);
    static void putField(Field_info* new_field,stack<Oop*>& op_stack);
    static void putStatic(Field_info* new_field,stack<Oop*>& op_stack,vm_thread& thread);
    static void invokeStatic(Method* new_method,stack<Oop*>& op_stack,vm_thread& thread,StackFrame& cur_frame,uint8_t*& pc);
    static void invokeVirtual(Method* new_method,stack<Oop*>& op_stack,vm_thread& thread,StackFrame& cur_frame,uint8_t*& pc);
};

#endif //AUTOMAN_JVM_BYTECODEENGINE_H
