//
// Created by 14394 on 2020/2/27.
//

#ifndef AUTOMAN_JVM_NONCOPYABLE_H
#define AUTOMAN_JVM_NONCOPYABLE_H


class noncopyable {
private:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;

protected:
    noncopyable()= default;
    ~noncopyable() = default;

};


#endif //AUTOMAN_JVM_NONCOPYABLE_H
