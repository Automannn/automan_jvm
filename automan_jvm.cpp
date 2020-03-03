//
// Created by 14394 on 2020/2/24.
//

#include "automan_jvm.h"
#include "runtime/kclass.h"
#include "native/java_lang_Class.h"
#include "native/java_lang_String.h"
#include "native/java_lang_Thread.h"
#include "native/native.h"
#include "system_directory.h"
#include "classloader.h"
#include "runtime/thread.h"
#include <regex>
#include "utils/synchronize_wcout.h"
#include <signal.h>
#include <process.h>

Lock thread_num_lock;
int all_thread_num;

//线程的任务
unsigned scapegoat (void *pp) {

    temp *real = (temp *)pp;
//	if (real->cur_thread_obj != nullptr) {		// so the ThreadTable::get_thread_obj may be nullptr.		// I add all thread into Table due to gc should stop all threads.
    //todo: 由于每次的 OpenThread打开的句柄，并不一致，因此修改所有的tid 为 id号
    ThreadTable::add_a_thread(GetCurrentThreadId(), real->cur_thread_obj, real->thread);		// the cur_thread_obj is from `java/lang/Thread.start0()`.
//	}
    if (real->should_be_stop_first) {		// if this thread is a child thread created by `start0`: should stop it first because of gc's race.
        // it will be hung up at the `global pthread_cond`. and will be wake up by `signal_all_thread()`.
        wait_cur_thread_and_set_bit(&real->the_first_wait_executed, real->thread);
    }
    real->thread->start(*real->arg);
    return 0;
};

void vm_thread::launch(InstanceOop *cur_thread_obj)
{
    // start one thread
    p.thread = this;
    p.arg = &const_cast<std::list<Oop *> &>(arg);
    p.cur_thread_obj = cur_thread_obj;
    if (cur_thread_obj != nullptr) {		// if arg is not nullptr, must be a thread created by `start0`.
        p.should_be_stop_first = true;
    }

    bool inited = automan_jvm::inited();
    //todo: 实际上这个线程是用于初始化的
   HANDLE cur_handle = (HANDLE)(_beginthreadex(NULL, 0, scapegoat, &p, 0, NULL));

    this->tid = GetThreadId(cur_handle);		// save to the vm_thread.
    if (!inited) {		// if this is the main thread which create the first init --> thread[0], then wait.
        //todo: 阻塞执行 tid线程，tid执行完后才往后执行
        WaitForSingleObject(cur_handle,INFINITE);
        GC::signal_all_patch();
        int remain_thread_num;
        while(true) {
            automan_jvm::num_lock().lock();
            {
                remain_thread_num = automan_jvm::thread_num();
            }
            automan_jvm::num_lock().unlock();

            assert(remain_thread_num >= 0);
            if (remain_thread_num == 0) {
                break;
            }
            //让出CPU调度
            Sleep(0);
        }
        GC::cancel_gc_thread();
        automan_jvm::end();
#ifdef DEBUG
        sync_wcout{} << pthread_self() << " run over!!!" << std::endl;		// delete
#endif
    }
}

void vm_thread::start(list<Oop *> & arg)
{
    if (automan_jvm::inited() == false) {
        assert(method == nullptr);			// if this is the init thread, method will be nullptr. this thread will get `main()` automatically.
        assert(arg.size() == 0);

        automan_jvm::inited() = true;			// important!
        //todo: 这里是执行 main 方法  ，重要
        vm_thread::init_and_do_main();		// init global variables and execute `main()` function.
    } else {
        // [x] if this is not the thread[0], detach itself is okay because no one will pthread_join it.
        //todo: 这里需要子线程分离，暂未找到合适的 api函数，因此先不做
//        pthread_detach(pthread_self());
        assert(this->vm_stack.size() == 0);	// check
        assert(arg.size() == 1);				// run() only has one argument `this`.

        this->vm_stack.push_back(StackFrame(method, nullptr, nullptr, arg, this));
        this->execute();

        automan_jvm::num_lock().lock();
        {
            automan_jvm::thread_num() --;
            assert(automan_jvm::thread_num() >= 0);
        }
        automan_jvm::num_lock().unlock();
    }

    WaitForSingleObject(_all_thread_wait_mutex,INFINITE);
    this->state = Death;
    ReleaseMutex(_all_thread_wait_mutex);
}

Oop *vm_thread::execute()
{
    Oop *all_jvm_return_value = nullptr;
    while(!vm_stack.empty()) {		// run over when stack is empty...
        StackFrame & cur_frame = vm_stack.back();
        if (cur_frame.method->is_native()) {
            pc = nullptr;
            // TODO: native.
            std::cerr << "Doesn't support native now." << std::endl;
            assert(false);
        } else {
            auto code = cur_frame.method->get_code();
            // TODO: support Code attributes......
            if (code->code_length == 0) {
                std::cerr << "empty method??" << std::endl;
                assert(false);		// for test. Is empty method valid ??? I dont know...
            }
            pc = code->code;
            Oop * return_val = BytecodeEngine::execute(*this, vm_stack.back(), this->thread_no);
            if (cur_frame.method->is_void()) {		// TODO: in fact, this can be delete. Because It is of no use.
                assert(return_val == nullptr);
                // do nothing
            } else {
                all_jvm_return_value = return_val;
                cur_frame.op_stack.push(return_val);
            }
        }
        vm_stack.pop_back();	// another half push_back() is in wind_jvm() constructor.
    }
    return all_jvm_return_value;
}

Oop * vm_thread::add_frame_and_execute(Method *new_method, const std::list<Oop *> & list) {
    // for defense:
    int frame_num = this->vm_stack.size();
    uint8_t *backup_pc = this->pc;
    this->vm_stack.push_back(StackFrame(new_method, this->pc, nullptr, list, this));
    Oop * result = BytecodeEngine::execute(*this, this->vm_stack.back(), this->thread_no);
    this->vm_stack.pop_back();
    this->pc = backup_pc;
    assert(frame_num == this->vm_stack.size());
    return result;
}


MirrorOop *vm_thread::get_caller_class_CallerSensitive()
{
    // back-trace. this method is called from: sun_reflect_Reflection.cpp:JVM_GetCallerClass()
    int level = 0;
    int total_levelnum = this->vm_stack.size();
    Method *m;
    for (list<StackFrame>::reverse_iterator it = this->vm_stack.rbegin(); it != this->vm_stack.rend(); ++it, ++level) {
        m = it->method;
        if (level == 0 || level == 1) {
            if (level == 0) {
                if (m->get_name() != L"getCallerClass" || m->get_descriptor() != L"()Ljava/lang/Class;")
                    assert(false);
            }
            // must be @CallerSensitive
            if (!m->has_annotation_name_in_method(L"Lsun/reflect/CallerSensitive;")) {
                assert(false);
            }
        } else {
            // TODO: openjdk: is_ignored_by_security_stack_walk(), but didn't implement the third switch because I didn't understand it...
            if (m->get_name() == L"invoke:(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;") {
                continue;	// java/lang/Reflection/Method.invoke(), ignore.
            } else if (m->get_klass()->check_parent(L"sun/reflect/MethodAccessorImpl")) {
                continue;
            } if (m->has_annotation_name_in_method(L"Lsun/reflect/CallerSensitive;")) {
                continue;
            } else {
                break;
            }
        }
    }
    if (level == total_levelnum) {		// no stackframe any more...
        assert(false);
    }
    m->print_all_attribute_name();		// delete
    return m->get_klass()->get_mirror();
}

void vm_thread::init_and_do_main()
{
    // init.
    {
        java_lang_class::init();		// must init !!!
        auto class_klass = BootStrapClassLoader::get_bootstrap().loadClass(L"java/lang/Class");
        java_lang_class::fixup_mirrors();	// only [basic types] + java.lang.Class + java.lang.Object

        // load String.class
        auto string_klass = ((InstanceKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"java/lang/String"));


        // 1. create a [half-completed] Thread obj, using the ThreadGroup obj.(for currentThread(), this must be create first!!)
        auto thread_klass = ((InstanceKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"java/lang/Thread"));
        InstanceOop *init_thread = thread_klass->new_instance();
        BytecodeEngine::initial_client(thread_klass, *this);		// first <clinit>!
        // inject!!
        //todo: 注意这里的 threadid 的来源，可能要修改
        init_thread->set_field_value(THREAD L":eetop:J", new LongOop((uint64_t)GetCurrentThreadId()));
        //todo: 这里的线程优先级还没有绑定到 thread句柄上， 通过 setThreadPriority
        //todo： 这里 ThreaPriority的属性值，暂时手写 norm为5，枚举在 native/thread里面
        init_thread->set_field_value(THREAD L":priority:I", new IntOop(5));
        ThreadTable::add_a_thread(GetCurrentThreadId(), init_thread, this);

        // 2. create a [System] ThreadGroup obj.
        auto threadgroup_klass = ((InstanceKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"java/lang/ThreadGroup"));
        InstanceOop *init_threadgroup = threadgroup_klass->new_instance();
        BytecodeEngine::initial_client(threadgroup_klass, *this);		// first <clinit>!
        {
            std::list<Oop *> list;
            list.push_back(init_threadgroup);	// $0 = this
            // execute method: java/lang/ThreadGroup.<init>:()V --> private Method!!
            Method *target_method = threadgroup_klass->get_this_class_method(L"<init>:()V");
            assert(target_method != nullptr);
            this->add_frame_and_execute(target_method, list);
        }
        // 3. INCOMPLETELY create a [Main] ThreadGroup obj.
        InstanceOop *main_threadgroup = threadgroup_klass->new_instance();
        {
            init_thread->set_field_value(THREAD L":group:Ljava/lang/ThreadGroup;", main_threadgroup);
        }
        assert(this->vm_stack.size() == 0);

        BytecodeEngine::initial_client(((InstanceKlass *)class_klass), *this);
        ((InstanceKlass *)class_klass)->set_static_field_value(L"useCaches:Z", new IntOop(false));

        // 3. load System class
        auto system_klass = ((InstanceKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"java/lang/System"));
        system_klass->set_state(Klass::KlassState::Initializing);
//		BytecodeEngine::initial_clinit(system_klass, *this);
        auto InputStream_klass = ((InstanceKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"java/io/InputStream"));
        BytecodeEngine::initial_client(InputStream_klass, *this);
        auto PrintStream_klass = ((InstanceKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"java/io/PrintStream"));
        BytecodeEngine::initial_client(PrintStream_klass, *this);
        auto SecurityManager_klass = ((InstanceKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"java/lang/SecurityManager"));
        BytecodeEngine::initial_client(SecurityManager_klass, *this);

        // 3.5 COMPLETELY create the [Main] ThreadGroup obj.
        {	// the second ThreadGroup, as openjdk
            std::list<Oop *> list;
            list.push_back(main_threadgroup);	// $0 = this
            list.push_back(nullptr);				// $1 = nullptr
            list.push_back(init_threadgroup);	// $2 = init_threadgroup
            list.push_back(java_lang_string::intern(L"main"));	// $3 = L"main"
            Method *target_method = threadgroup_klass->get_this_class_method(L"<init>:(Ljava/lang/Void;Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
            assert(target_method != nullptr);
            this->add_frame_and_execute(target_method, list);
        }

        // 3.7 do not support Debug class
        auto Security_DEBUG_klass = ((InstanceKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"sun/security/util/Debug"));
        Security_DEBUG_klass->set_state(Klass::KlassState::Initializing);

        // 4. [complete] the Thread obj using the [uncomplete] main_threadgroup.
        {
            std::list<Oop *> list;
            list.push_back(init_thread);			// $0 = this
            list.push_back(main_threadgroup);	// $1 = [main_threadGroup]
            list.push_back(java_lang_string::intern(L"main"));	// $2 = L"main"
            // execute method: java/lang/Thread.<init>:(ThreadGroup, String)V --> public Method.
            Method *target_method = thread_klass->get_this_class_method(L"<init>:(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
            assert(target_method != nullptr);
            this->add_frame_and_execute(target_method, list);
        }

        // 3.3 Complete! invoke the method...
        // java/lang/System::initializeSystemClass(): "Initialize the system class.  Called after thread initialization." So it must be created after the thread.
        Method *_initialize_system_class = system_klass->get_this_class_method(L"initializeSystemClass:()V");
        this->add_frame_and_execute(_initialize_system_class, {});
        system_klass->set_state(Klass::KlassState::Initialized);		// set state.

        // 3.7 Complete!
        BytecodeEngine::initial_client(SecurityManager_klass, *this);
        Security_DEBUG_klass->set_state(Klass::KlassState::Initialized);
    }

    auto Perf_klass = ((InstanceKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"sun/misc/Perf"));
    Perf_klass->set_state(Klass::KlassState::Initializing);				// ban Perf.
    auto PerfCounter_klass = ((InstanceKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"sun/misc/PerfCounter"));
    PerfCounter_klass->set_state(Klass::KlassState::Initializing);		// ban PerfCounter.

    auto launcher_helper_klass = ((InstanceKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"sun/launcher/LauncherHelper"));
    BytecodeEngine::initial_client(launcher_helper_klass, *this);
    Method *load_main_method = launcher_helper_klass->get_this_class_method(L"checkAndLoadMain:(ZILjava/lang/String;)Ljava/lang/Class;");
    // new a String.
    InstanceOop *main_klass = (InstanceOop *)java_lang_string::intern(automan_jvm::main_class_name());

    this->vm_stack.push_back(StackFrame(load_main_method, nullptr, nullptr, {new IntOop(true), new IntOop(1), main_klass}, this));
    MirrorOop *main_class_mirror = (MirrorOop *)this->execute();
    assert(main_class_mirror->get_ooptype() == OopType::_InstanceOop);

    // first execute <clinit> if has
    BytecodeEngine::initial_client(((InstanceKlass *)main_class_mirror->get_mirrored_who()), *this);
    // get the `public static void main()`.
    auto main_method = ((InstanceKlass *)main_class_mirror->get_mirrored_who())->get_static_void_main();
    // new a String[], for the arguments.
    ObjArrayOop *string_arr_oop = (ObjArrayOop *)((ObjArrayKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"[Ljava/lang/String;"))->new_instance(automan_jvm::argv().size());
    auto iter = system_classmap.find(L"java/lang/String.class");
    assert(iter != system_classmap.end());
    auto string_klass = ((InstanceKlass *)(*iter).second);
    for (int i = 0; i < automan_jvm::argv().size(); i ++) {
        (*string_arr_oop)[i] = java_lang_string::intern(automan_jvm::argv()[i]);
    }

    // load some useful klass...
    {
        auto methodtype_klass = ((InstanceKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"java/lang/invoke/MethodType"));
        BytecodeEngine::initial_client(methodtype_klass, *this);
        auto methodhandle_klass = ((InstanceKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"java/lang/invoke/MethodHandle"));
        BytecodeEngine::initial_client(methodhandle_klass, *this);

    }
    // The World's End!
    this->vm_stack.push_back(StackFrame(main_method, nullptr, nullptr, {string_arr_oop}, this));
    this->execute();
}

ArrayOop * vm_thread::get_stack_trace()
{
    auto stack_elem_arr_klass = ((ObjArrayKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"[Ljava/lang/StackTraceElement;"));
    auto stack_elem_klass = ((InstanceKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"java/lang/StackTraceElement"));
    auto stack_elem_init = stack_elem_klass->get_this_class_method(L"<init>:(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    assert(stack_elem_init != nullptr);

    auto arr = (ArrayOop *)stack_elem_arr_klass->new_instance(this->vm_stack.size());

    std::wstringstream ss;

    uint8_t *last_frame_pc = this->pc;
    int i = 0;
    for (list<StackFrame>::reverse_iterator it = this->vm_stack.rbegin(); it != this->vm_stack.rend(); ++it) {
        Method *m = it->method;
        auto klass_name = java_lang_string::intern(m->get_klass()->get_name());
        auto method_name = java_lang_string::intern(m->get_name());
        wstring java_file_name = m->get_klass()->get_source_file_name();
        auto file_name = java_lang_string::intern(java_file_name);

        int line_num;
        int last_pc_debug = 0;
        if (java_file_name == L"") {		// must be VM Anonymous Klass...
            line_num = 0;
        } else {
            if (last_frame_pc == 0)
                line_num = 0;
            else {
                last_pc_debug = last_frame_pc - m->get_code()->code;
                line_num = m->get_java_source_lineno(last_frame_pc - m->get_code()->code);
            }

        }
        // set next frame's pc
        last_frame_pc = it->return_pc;
        // inject
        (*arr)[i] = stack_elem_klass->new_instance();
        ((InstanceOop *)(*arr)[i])->set_field_value(STACKTRACEELEMENT L":declaringClass:" STR, klass_name);
        ((InstanceOop *)(*arr)[i])->set_field_value(STACKTRACEELEMENT L":methodName:" STR,     method_name);
        ((InstanceOop *)(*arr)[i])->set_field_value(STACKTRACEELEMENT L":fileName:" STR,       file_name);
        ((InstanceOop *)(*arr)[i])->set_field_value(STACKTRACEELEMENT L":lineNumber:I",        new IntOop(line_num));

#ifdef DEBUG
        ss << "[backtrace " << this->vm_stack.size() - i - 1 << "] pc: [" << last_pc_debug << "], at <" << m->get_klass()->get_name() << ">::[" << m->get_name() << ":" << m->get_descriptor() << "], at [" << m->get_klass()->get_source_file_name() << "], line [" << line_num << "]." << std::endl;
	int j = 1;
	for (Oop * value : it->localVariableTable) {
		ss << "    the localVariableTable[" << j-1 << "] is " << it->print_arg_msg(value, this) << std::endl;
		if (value != nullptr && value->get_ooptype() == OopType::_BasicTypeOop
						&& ((((BasicTypeOop *)value)->get_type() == Type::LONG) || (((BasicTypeOop *)value)->get_type() == Type::DOUBLE))) {
			j ++;		// jump. because `long` and `double` take 2 places.
		}
		j ++;
	}
#endif

        i ++;
    }

#ifdef DEBUG
    bool _switch_ = sync_wcout::_switch();
	sync_wcout::set_switch(true);
	sync_wcout{} << "===-------------------------------------- printStackTrace() -----------------------------------------===" << std::endl;
	sync_wcout{} << ss.str();
	sync_wcout{} << "===--------------------------------------------------------------------------------------------------===" << std::endl;
	sync_wcout::set_switch(_switch_);
#endif
    return arr;
}

void vm_thread::set_exception_at_last_second_frame() {
    assert(this->vm_stack.size() >= 2);
    auto iter = this->vm_stack.rbegin();
    ++iter;							// get cur_frame's prev...
    iter->has_exception = true;
}

HANDLE _all_thread_wait_mutex;
HANDLE _all_thread_wait_cond;

void wait_cur_thread(vm_thread *thread)
{
    WaitForSingleObject(_all_thread_wait_mutex,INFINITE);
    thread->set_state(Waiting);
    while (true) {
        bool gc;
        GC::gc_lock().lock();
        {
            gc = GC::gc();
        }
        GC::gc_lock().unlock();

        if (gc) {
            //todo: gc完成后，阻塞在所有线程等待上（可能有gc线程触发）
            WaitForSingleObject(_all_thread_wait_cond, INFINITE);
            ReleaseMutex(_all_thread_wait_mutex);
        } else {
            break;
        }

    }
    thread->set_state(Running);
    ReleaseMutex(_all_thread_wait_mutex);
}

void wait_cur_thread_and_set_bit(volatile bool *bit, vm_thread *thread)
{
    WaitForSingleObject(_all_thread_wait_mutex,INFINITE);
    *bit = true;
    thread->set_state(Waiting);
    WaitForSingleObject(_all_thread_wait_cond, INFINITE);
    thread->set_state(Running);
    ReleaseMutex(_all_thread_wait_mutex);
}


void signal_one_thread()
{
    SetEvent(_all_thread_wait_cond);
}

void signal_all_thread()
{
   int size = automan_jvm::thread_num();
    for (int i = 0; i < size; ++i) {
        SetEvent(_all_thread_wait_cond);
        //todo: 这里通过释放 CPU 达到broadst的目的
        Sleep(0);
    }
}

void SIGINT_handler(int signo)
{
    // re-use gc bit to stop-the-world，but won't trigger GC。
    while (true) {
        bool gc;
        GC::gc_lock().lock();
        {
            gc = GC::gc();
        }
        GC::gc_lock().unlock();
        if (gc) {
            continue;
        } else {
            GC::gc_lock().lock();
            {
                GC::gc() = true;
            }
            GC::gc_lock().unlock();
            // FIXME: I don't know whether it is safe... only a solution for dead lock of wind_jvm::num_lock...
            automan_jvm::num_lock().unlock();		// It's only a patch.
            GC::detect_ready();
            GC::gc() = false;		// set back
            BytecodeEngine::main_thread_exception();		// exit
        }
    }
}

void automan_jvm::run(const wstring & main_class_name, const vector<wstring> & argv)
{
    //todo: 这里注册了一个交互信号，信号处理程序又是一个 无限循环，需要注意
    //todo: 用 raise 生成信号
    signal(SIGINT, SIGINT_handler);

    automan_jvm::main_class_name() = std::regex_replace(main_class_name, std::wregex(L"\\."), L"/");
    automan_jvm::argv() = const_cast<vector<wstring> &>(argv);

    vm_thread *init_thread;
    automan_jvm::lock().lock();
    {
        automan_jvm::threads().push_back(vm_thread(nullptr, {}));
        init_thread = &automan_jvm::threads().back();
    }
    automan_jvm::lock().unlock();

    init_native();

    HANDLE gc_tid;
    gc_tid= (HANDLE)(_beginthreadex(nullptr, 0, reinterpret_cast<unsigned int (*)(void *)>(GC::gc_thread), NULL, 0, NULL));
    gc_thread() = gc_tid;
    // go!
    init_thread->launch();		// begin this thread.
}

void automan_jvm::end()
{
    MethodPool::cleanup();
    Field_Pool::cleanup();

    ClassFile_Pool::cleanup();
    Rt_Pool::cleanup();				// rt pool may be must deallocate before MyClassLoader...?

    BootStrapClassLoader::get_bootstrap().cleanup();
    AutomanClassLoader::get_loader().cleanup();
    // finally! delete all allocated memory!!
    MemAlloc::cleanup();
}

