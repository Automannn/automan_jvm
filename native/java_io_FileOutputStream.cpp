//
// Created by 14394 on 2020/2/26.
//

#include "../automan_jvm.h"
#include "java_io_FileOutputStream.h"
#include <vector>
#include <algorithm>
#include <cassert>
#include "native.h"
#include <unistd.h>
#include "../runtime/thread.h"
#include <string.h>
#include <dhcpcsdk.h>

#define MAX_BUF_SIZE		4096
static unordered_map<wstring, void*> methods = {
        {L"initIDs:()V",				(void *)&JVM_FOS_InitIDs},
        {L"writeBytes:([BIIZ)V",		(void *)&JVM_WriteBytes},
};

void JVM_FOS_InitIDs(list<Oop *> & _stack){		// static
    // do nothing...
}

void JVM_WriteBytes(list<Oop *> & _stack){
    InstanceOop *_this = (InstanceOop *)_stack.front();	_stack.pop_front();
    TypeArrayOop *bytes = (TypeArrayOop *)_stack.front(); _stack.pop_front();
    int offset = ((IntOop *)_stack.front())->value;	_stack.pop_front();
    int len = ((IntOop *)_stack.front())->value;	_stack.pop_front();
    bool append = (bool)((IntOop *)_stack.front())->value;	_stack.pop_front();		// in linux/unix, append is of no use. because `append` is `open()`'s property in *nix. It's only useful for windows.

    Oop *oop;
    // get the unix fd.
    //todo: 注意，当windos调用标准输入输出流的时候，通过handle进行读写，此时的FileDescriptor 的 fd 属性为 -1
    _this->get_field_value(FILEOUTPUTSTREAM L":fd:Ljava/io/FileDescriptor;", &oop);
    ((InstanceOop *)oop)->get_field_value(FILEDESCRIPTOR L":fd:I", &oop);
    int fd = ((IntOop *)oop)->value;
    //注意，因为这里的Oop用的是同一个地址，所以，需要再次调用
    _this->get_field_value(FILEOUTPUTSTREAM L":fd:Ljava/io/FileDescriptor;", &oop);
    ((InstanceOop *)oop)->get_field_value(FILEDESCRIPTOR L":handle:J", &oop);
    long handle =((LongOop *)oop)->value;
    assert(bytes->get_length() > offset && bytes->get_length() >= (offset + len));		// ArrayIndexOutofBoundException
//    if (fd == STDOUT_FILENO) {
    if (fd == -1) {
        //todo:表明此时写入的目的地是 标准输入流，输出流，或者是 标准错误流
        assert(handle!=-1);
        char *buf = new char[5+len+5];
        switch (ThreadTable::get_threadno(GetCurrentThreadId()) % 7) { //根据线程的id号，输出不同的颜色
            case 0:
                strncpy(buf, RESET, 5);		// do not copy the final '\0'.
                break;
            case 1:
                strncpy(buf, RED, 5);
                break;
            case 2:
                strncpy(buf, GREEN, 5);
                break;
            case 3:
                strncpy(buf, YELLOW, 5);
                break;
            case 4:
                strncpy(buf, BLUE, 5);
                break;
            case 5:
                strncpy(buf, MAGENTA, 5);
                break;
            case 6:
                strncpy(buf, CYAN, 5);
                break;
            case -1:
                strncpy(buf, RESET, 5);		// for the C++ `int main()`. this is the first thread, which is not created by manually `pthread_create`.
                break;
        }
        int j = 5;
        for (int i = offset; i < offset + len; i ++, j ++) {
            buf[j] = (char)((IntOop *)(*bytes)[i])->value;
        }
        strncpy(buf+j, RESET, 5);
        HANDLE _handle =LongToHandle(handle);
        //todo: 不知道标准输出流需不需要加上偏移位置？
        DWORD iWrite =0;
        bool ret= WriteFile(_handle,buf,5+len+5,&iWrite,NULL);
        if (!ret) {
            assert(false);
        }
        delete[] buf;
    } else {
        //表明当前写入的目的地是 文件描述符，也就是文件系统
        char *buf = new char[len];
        for (int i = offset, j = 0; i < offset + len; i ++, j ++) {
            buf[j] = (char)((IntOop *)(*bytes)[i])->value;
        }
        if (write(fd, buf, len) == -1) {
            assert(false);
        }
        delete[] buf;
    }
}

void *java_io_fileOutputStream_search_method(const wstring & signature)
{
    auto iter = methods.find(signature);
    if (iter != methods.end()) {
        return (*iter).second;
    }
    return nullptr;
}
