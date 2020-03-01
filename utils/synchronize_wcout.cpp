//
// Created by 14394 on 2020/2/23.
//

#include "synchronize_wcout.h"
#include <windows.h>

sync_wcout::~sync_wcout() {
    std::lock_guard<std::mutex> guard(mutex());
    if (!_switch()) {
        return;
    }
    //todo: 这里的 thread号 没有调用 thread的 get_thread_no
    switch (GetCurrentThreadId() % 7) {
        case 0:
            std::wcout << RESET << this->str();
            break;
        case 1:
            std::wcout << RED << this->str();
            break;
        case 2:
            std::wcout << GREEN << this->str();
            break;
        case 3:
            std::wcout << YELLOW << this->str();
            break;
        case 4:
            std::wcout << BLUE << this->str();
            break;
        case 5:
            std::wcout << MAGENTA << this->str();
            break;
        case 6:
            std::wcout << CYAN << this->str();
            break;
    }
}