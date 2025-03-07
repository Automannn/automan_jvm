//
// Created by 14394 on 2020/2/23.
//

#ifndef AUTOMAN_JVM_SYNCHRONIZE_WCOUT_H
#define AUTOMAN_JVM_SYNCHRONIZE_WCOUT_H

#include <iostream>
#include <sstream>
#include <mutex>

#include <iostream>
#include <sstream>
#include <mutex>

// from Online Source. Unix/Linux only. Used for different thead's output.
#define RESET		"\033[00m"
//#define BLACK		"\033[30m"      	/* Black */
#define RED			"\033[31m"      /* Red */
#define GREEN		"\033[32m"      /* Green */
#define YELLOW		"\033[33m"      /* Yellow */
#define BLUE			"\033[34m"      /* Blue */
#define MAGENTA		"\033[35m"      /* Magenta */
#define CYAN			"\033[36m"      /* Cyan */

/**
 * from Stack Overflow: https://stackoverflow.com/questions/14718124/how-to-easily-make-stdcout-thread-safe
 * Though there has lots of object to be generated, it is a good way to sove the synchronized problem ~
 */
class sync_wcout : public std::wostringstream {
private:
    static std::mutex& mutex(){
        static std::mutex mutex;
        return mutex;
    }

public:
    static bool& _switch(){
        static  bool _switch = false;
        return _switch;
    }

    static void set_switch(bool open){
        std::lock_guard<std::mutex> guard(mutex());
        _switch() = open;
    }

public:
    sync_wcout() = default;
    ~sync_wcout();
};


#endif //AUTOMAN_JVM_SYNCHRONIZE_WCOUT_H
