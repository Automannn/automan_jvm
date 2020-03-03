//
// Created by 14394 on 2020/3/3.
//

#ifndef AUTOMAN_JVM_JAVA_UTIL_ZIP_ZIPFILE_H
#define AUTOMAN_JVM_JAVA_UTIL_ZIP_ZIPFILE_H


#include <list>
#include "../runtime/oop.h"

using std::list;

void JVM_ZIP_initIDs(list<Oop*>& _stack);

void *java_util_zip_ZipFile_search_method(const wstring& signature);


#endif //AUTOMAN_JVM_JAVA_UTIL_ZIP_ZIPFILE_H
