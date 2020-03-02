//
// Created by 14394 on 2020/2/23.
//

#include "utils.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <locale>
#include <windows.h>

using namespace std;

string  wstring_to_utf8(const wstring& wstr){
    LPCWSTR pwszSrc = wstr.c_str();
    int nLen = WideCharToMultiByte( CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL );
    if ( nLen == 0 )
        return(std::string( "" ) );
    char* pszDst = new char[nLen];
    if ( !pszDst )
        return(std::string( "" ) );
    WideCharToMultiByte( CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL );
    std::string str( pszDst );
    delete[] pszDst;
    pszDst = NULL;
    return(str);
}

wstring utf8_to_wstring(const string& str){
    LPCSTR  pszSrc  = str.c_str();
    int nLen = MultiByteToWideChar( CP_ACP, 0, pszSrc, -1, NULL, 0 );
    if ( nLen == 0 )
        return(std::wstring( L"" ) );

    wchar_t* pwszDst = new wchar_t[nLen];
    if ( !pwszDst )
        return(std::wstring( L"" ) );
    MultiByteToWideChar( CP_ACP, 0, pszSrc, -1, pwszDst, nLen );
    std::wstring wstr( pwszDst );
    delete[] pwszDst;
    pwszDst = NULL;
    return(wstr);
}

vector<wstring>& split(wstring str,wstring splitter){
    vector<wstring>* ret=new vector<wstring>();
    int seprate_len=splitter.length();
    int start=0;
    int index;
    while((index = str.find(splitter,start))!=-1){
        ret->push_back(str.substr(start,index-start));
        start = index+seprate_len;
    }
    if(start<str.length())
        ret->push_back(str.substr(start,str.length()-start));
    return *ret;
}
