//
// Created by 14394 on 2020/2/29.
//
#include "automan_jvm.h"
#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{
//#ifdef DEBUG
    sync_wcout::set_switch(true);
//#endif
    if (argc != 2) {
        std::wcerr << "argc is not 2. please re-run." << std::endl;
        exit(-1);
    }

    wstring program = utf8_to_wstring(std::string(argv[1]));

    std::ios::sync_with_stdio(true);		// keep thread safe?
    std::wcout.imbue(std::locale(""));
    std::vector<std::wstring> v{ L"automan_jvm", L"1", L"2" };
    automan_jvm::run(program, v);
}