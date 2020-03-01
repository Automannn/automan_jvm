//
// Created by 14394 on 2020/2/27.
//

#include "../automan_jvm.h"
#include "native.h"
#include "java_lang_Object.h"
#include "java_lang_Thread.h"
#include "java_lang_System.h"
#include "java_lang_Class.h"
#include "sun_misc_Unsafe.h"
#include "sun_reflect_Reflection.h"
#include "java_security_AccessController.h"
#include "java_lang_Float.h"
#include "java_lang_Double.h"
#include "sun_misc_VM.h"
#include "java_io_FileInputStream.h"
#include "java_io_FileDescriptor.h"
#include "java_io_FileOutputStream.h"
#include "java_lang_String.h"
#include "sun_reflect_NativeConstrctorAccessorImpl.h"
#include "java_util_concurrent_atomic_AtomicLong.h"
#include "java_io_UnixFileSystem.h"
#include "sun_misc_signal.h"
#include "sun_misc_URLClassPath.h"
#include "java_lang_ClassLoader.h"
#include "java_lang_Runtime.h"
#include "java_lang_Throwable.h"
#include "java_io_FileSystem.h"
#include "java_lang_Package.h"
#include "java_lang_invoke_MethodHandleNatives.h"
#include "java_lang_reflect_Array.h"
#include "java_lang_invoke_MethodHandle.h"
#include "sun_reflect_NativeMethodAccessorImpl.h"
#include "java_lang_Shutdown.h"

static unordered_map<wstring, function<void*(const wstring &)>> native_map;		// such as: {L"java/lang/Object", search [native method]'s method lambda for java/lang/Object}

void init_native()		// the same as "registerNatives" method.
{
    native_map[L"java/lang/Object"] = java_lang_object_search_method;
    native_map[L"java/lang/System"] = java_lang_system_search_method;
    native_map[L"java/lang/Thread"] = java_lang_thread_search_method;
    native_map[L"java/lang/Class"] = java_lang_class_search_method;
    native_map[L"sun/misc/Unsafe"] = sun_misc_unsafe_search_method;
    native_map[L"sun/reflect/Reflection"] = sun_reflect_reflection_search_method;
    native_map[L"java/security/AccessController"] = java_security_accesscontroller_search_method;
    native_map[L"java/lang/Float"] = java_lang_float_search_method;
    native_map[L"java/lang/Double"] = java_lang_double_search_method;
    native_map[L"sun/misc/VM"] = sun_misc_vm_search_method;
    native_map[L"java/io/FileInputStream"] = java_io_fileInputStream_search_method;
    native_map[L"java/io/FileDescriptor"] = java_io_fileDescriptor_search_method;
    native_map[L"java/io/FileOutputStream"] = java_io_fileOutputStream_search_method;
    native_map[L"java/lang/String"] = java_lang_string_search_method;
    native_map[L"sun/reflect/NativeConstructorAccessorImpl"] = sun_reflect_nativeConstructorAccessorImpl_search_method;
    native_map[L"java/util/concurrent/atomic/AtomicLong"] = java_util_concurrent_atomic_atomicLong_search_method;
    native_map[L"java/io/UnixFileSystem"] = java_io_unixFileSystem_search_method;
    native_map[L"sun/misc/Signal"] = sun_misc_signal_search_method;
    native_map[L"sun/misc/URLClassPath"] = sun_misc_urlClassPath_search_method;
    native_map[L"java/lang/ClassLoader"] = java_lang_classLoader_search_method;
    native_map[L"java/lang/Runtime"] = java_lang_runtime_search_method;
    native_map[L"java/lang/Throwable"] = java_lang_throwable_search_method;
    native_map[L"java/io/FileSystem"] = java_io_fileSystem_search_method;
    native_map[L"java/lang/Package"] = java_lang_package_search_method;
    native_map[L"java/lang/invoke/MethodHandleNatives"] = java_lang_invoke_methodHandleNatives_search_method;
    native_map[L"java/lang/reflect/Array"] = java_lang_reflect_array_search_method;
    native_map[L"java/lang/invoke/MethodHandle"] = java_lang_invoke_methodHandle_search_method;
    native_map[L"sun/reflect/NativeMethodAccessorImpl"] = sun_reflect_nativeMethodAccessorImpl_search_method;
    native_map[L"java/lang/Shutdown"] = java_lang_shutdown_search_method;
}

// find a native method <$signature> in a klass <$klass_name>, return the method in (void *). if didn't find, abort().
void *find_native(const wstring & klass_name, const wstring & signature)	// such as: find_native(L"java/lang/Object", L"notify:()V")
{
    auto iter = native_map.find(klass_name);
    if (iter != native_map.end()) {
        return (*iter).second(signature);		// call the klass's find native method.	// maybe will get nullptr.
    } else {
        std::wcerr << "didn't find [" << klass_name << ":" << signature << "] in native !! it didn't do registerNatives() function!!" << std::endl;
        assert(false);
    }
}

void native_throw_Exception(InstanceKlass *excp_klass, vm_thread *thread, list<Oop *> & _stack, const std::wstring & msg)
{
    auto excp_obj = excp_klass->new_instance();
    thread->set_exception_at_last_second_frame();		// set exception.
    auto init_method = excp_klass->get_this_class_method(L"<init>:(Ljava/lang/String;)V");
    assert(init_method != nullptr);
    thread->add_frame_and_execute(init_method, {excp_obj, java_lang_string::intern(msg)});
    _stack.push_back(excp_obj);
}