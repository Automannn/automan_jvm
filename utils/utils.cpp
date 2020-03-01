//
// Created by 14394 on 2020/2/23.
//

#include "utils.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <locale>

using namespace std;

string  wstring_to_utf8(const wstring& str){
    wstring_convert<codecvt_utf8< wchar_t>> myconv;
    return myconv.to_bytes(str);
}

wstring utf8_to_wstring(const string& str){
    wstring_convert<codecvt_utf8< wchar_t>> myconv;
    return myconv.from_bytes(str);
}

vector<wstring>& split(wstring str,wstring splitter){
    vector<wstring> ret;
    int seprate_len=splitter.length();
    int start=0;
    int index;
    while((index = str.find(splitter,start))!=-1){
        ret.push_back(str.substr(start,index-start));
        start = index+seprate_len;
    }
    if(start<str.length())
        ret.push_back(str.substr(start,str.length()-start));
    return ret;
}
