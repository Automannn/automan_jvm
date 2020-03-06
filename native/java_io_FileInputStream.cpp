//
// Created by 14394 on 2020/2/26.
//

#include "../automan_jvm.h"
#include "java_io_FileInputStream.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include "native.h"
#include "java_lang_String.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <istream>
#include <errno.h>
#include "../classloader.h"

static unordered_map<wstring, void*> methods = {
        {L"initIDs:()V",				(void *)&JVM_FIS_InitIDs},
        {L"open0:(" STR ")V",		(void *)&JVM_Open0},
        {L"readBytes:([BII)I",		(void *)&JVM_ReadBytes},
        {L"close0:()V",				(void *)&JVM_Close0},
};

void JVM_FIS_InitIDs(list<Oop *> & _stack){		// static
    // do nothing...
}

void JVM_Open0(list<Oop *> & _stack){
    InstanceOop *_this = (InstanceOop *)_stack.front();	_stack.pop_front();
    InstanceOop *str = (InstanceOop *)_stack.front();	_stack.pop_front();
    std::string backup_str = wstring_to_utf8(java_lang_string::stringOop_to_wstring(str));
    std::cout<<"access file: "<<backup_str<<std::endl;
    const char *filename = backup_str.c_str();
#ifdef DEBUG
    sync_wcout{} << "open filename: [" << filename << "]." << std::endl;
#endif
    // use O_RDONLY to open the file!!! (of FileInputStream, 1. readOnly, 2. only read File, not dir!)
    int fd;
    do {
        fd = open(filename, O_RDONLY|O_BINARY, 1);
        //这里先将之前的文件打开方式给替换掉
//        fd = open(filename, O_RDONLY, 0666);
    } while (fd == -1 && errno == EINTR);		// prevent from being interrupted from SIGINT.
    if (fd == -1)	assert(false);
    int ret;
    struct stat64 stat;
    do {
        ret = fstat64(fd, &stat);
    } while (ret == -1 && errno == EINTR);
    if (ret == -1)	assert(false);
    if (S_ISDIR(stat.st_mode)) {		// check whether `fd` is a dir. FileInputStream.open() can only open the `file`, not `dir`.
        close(fd);
        // throw FileNotFoundException: xxx is a directory.
        // get the exception klass
        auto excp_klass = ((InstanceKlass *)BootStrapClassLoader::get_bootstrap().loadClass(L"java/io/FileNotFoundException"));
        // get current thread
        vm_thread *thread = (vm_thread *)_stack.back();	_stack.pop_back();
        // make a message
        std::string msg0(filename);
        wstring msg = utf8_to_wstring(msg0);
        msg += L" (Is a directory)";
        // go!
        native_throw_Exception(excp_klass, thread, _stack, msg);	// the exception obj has been save into the `_stack`!
        return;
    }
    // inject into FileInputStream.fd !!
    Oop *result;
    _this->get_field_value(FILEINPUTSTREAM L":fd:Ljava/io/FileDescriptor;", &result);
    Oop *real_fd;
    ((InstanceOop *)result)->get_field_value(FILEDESCRIPTOR L":fd:I", &real_fd);
#ifdef DEBUG
    sync_wcout{} << "(DEBUG) open a file [" << filename << "], and inject real fd: [" << fd << "] to substitude the old garbage value fd: [" << ((IntOop *)real_fd)->value << "]." << std::endl;
#endif
    ((IntOop *)real_fd)->value = fd;		// inject the real fd into it!
}

void JVM_ReadBytes(list<Oop *> & _stack){
    InstanceOop *_this = (InstanceOop *)_stack.front();	_stack.pop_front();
    TypeArrayOop *bytes = (TypeArrayOop *)_stack.front(); _stack.pop_front();
    int offset = ((IntOop *)_stack.front())->value;	_stack.pop_front();
    int len = ((IntOop *)_stack.front())->value;	_stack.pop_front();
    bool append = (bool)((IntOop *)_stack.front())->value;	_stack.pop_front();		// in linux/unix, append is of no use. because `append` is `open()`'s property in *nix. It's only useful for windows.

    Oop *oop;
    // get the unix fd.
    _this->get_field_value(FILEINPUTSTREAM L":fd:Ljava/io/FileDescriptor;", &oop);
    ((InstanceOop *)oop)->get_field_value(FILEDESCRIPTOR L":fd:I", &oop);
    int fd = ((IntOop *)oop)->value;
    assert(bytes->get_length() > offset && bytes->get_length() >= (offset + len));		// ArrayIndexOutofBoundException
    char *buf = new char[len];
    //TODO:这里有一个大大大大大大！！！！ 坑
    //windows中，读取的0x1A 会触发 EOF，经检测，256个字节中，只有0x1A会造成此异常，因此，采用fseek重定位指针处理
    int ret;
    if ((ret = read(fd, buf, len)) == -1) {
        assert(false);
    }
    //todo: 同时要处理，多次读取的指针偏移  即 append的作用
    //处理文件中存在 0x1A的情况,注意，当前的len并不一定是文件的长度，而是该次读取的长度，其肯定是小于等于文件长度的
    while (ret<len){
        buf[ret-1]='\032';//填充 0x1A
        lseek(fd,1,SEEK_CUR);ret++;//向后偏移一个位置后，修正读取数量
        int _subLen= len-ret; //剩余的字节长度
        char* _subBuf = new char[_subLen];
        int beforeLen = ret;
        int afterLen =read(fd,_subBuf,_subLen);
        if(afterLen==0) continue; //如果之后读取到的依然为0x1A,则跳过当次循环，进入下一次处理
        for(int i=beforeLen-1,j=0;i<beforeLen+afterLen;i++,j++){//将新读取的内容进行拷贝
            buf[i]=_subBuf[j];
        }
        ret = beforeLen+afterLen; //拷贝完成后，修正已读数量
    }

    if (ret == 0) {		// EOF
        _stack.push_back(new IntOop(-1));
#ifdef DEBUG
        sync_wcout{} << "(DEBUG) meet EOF of fd: [" << fd << "]!" << std::endl;
#endif
    } else {				// Not EOF
        for (int i = offset, j = 0; i < offset + ret; i ++, j ++) {		// I think it should be `offset + ret`, not `offset + len`.
            ((IntOop *)(*bytes)[i])->value = buf[j];
        }
        _stack.push_back(new IntOop(ret));
#ifdef DEBUG
        sync_wcout{} << "(DEBUG) read fd: [" << fd << "] for [" << ret << "] bytes." << std::endl;
#endif
    }
    delete[] buf;
}

void JVM_Close0(list<Oop *> & _stack){
    InstanceOop *_this = (InstanceOop *)_stack.front();	_stack.pop_front();
    Oop *oop;
    // get the unix fd.
    _this->get_field_value(FILEINPUTSTREAM L":fd:Ljava/io/FileDescriptor;", &oop);
    ((InstanceOop *)oop)->get_field_value(FILEDESCRIPTOR L":fd:I", &oop);
    int fd = ((IntOop *)oop)->value;
    assert(fd != -1);
    if (close(fd) == -1) {
        assert(false);
    }
#ifdef DEBUG
    sync_wcout{} << "(DEBUG) close a file fd: [" << fd << "]." << std::endl;
#endif
}

void *java_io_fileInputStream_search_method(const wstring & signature)
{
    auto iter = methods.find(signature);
    if (iter != methods.end()) {
        return (*iter).second;
    }
    return nullptr;
}