//
// Created by 14394 on 2020/2/24.
//

#ifndef AUTOMAN_JVM_JARLISTER_H
#define AUTOMAN_JVM_JARLISTER_H

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <cassert>
#include "utils/utils.h"
#include <io.h>

using std::shared_ptr;
using std::wstring;
using std::set;
using std::vector;
using std::unordered_set;

extern wstring splitter;

class StringSplitter{
private:
    vector<wstring> splits;
    int cur = 0; //counter for finding in RtJarDirectory
public:
    StringSplitter(const wstring& s){
        splits = split(s,splitter);
    }

    const vector<wstring>& result(){
        return splits;
    }

    int& counter(){
        return cur;
    }
};

struct shared_RtJarDirectory_compare;

class RtJarDirectory{
private:
    wstring name;
    shared_ptr<set<shared_ptr<RtJarDirectory>,shared_RtJarDirectory_compare>> subdir; //sub directory
public:
    explicit RtJarDirectory(const wstring& filename);

private:
    shared_ptr<RtJarDirectory> findFolderInThis(const wstring& s) const;
    void print(int level) const;

public:
    const wstring& get_name() const{return name;};
    void add_file(StringSplitter&& ss);
    bool find_file(StringSplitter&& ss) const;
    void print() const;

public:
    bool operator<(const RtJarDirectory& rhs) const{ //for set sort
        return this->name<rhs.name;
    }

    bool operator < (const wstring & rhs) const {	// for C++14 subdir.find(wstring)
        return this->name < rhs;
    }

    friend bool operator<(const wstring& lhs,const RtJarDirectory& rhs){
        return lhs < rhs.name;
    }

    friend bool operator<(const shared_ptr<RtJarDirectory>& lhs,const shared_ptr<RtJarDirectory>& rhs){
        return *lhs<*rhs;
    }
};

extern const unordered_set<wstring> exclude_files;

class Filter{
public:
    static bool filt(const wstring& s){
        bool have1 = exclude_files.find(s)!=exclude_files.end();
        bool have2 = false;
        for(const wstring& ss: exclude_files){
            if(s.find(ss)!=-1){
                have2 = true;
                break;
            }
        }
        return have1||have2;
    }
};

//get pwd:
extern wstring pwd;

class JarLister{
private:
    wstring rtjar_pos;
    RtJarDirectory rjd;
    const wstring rtlist = L"rt.list";
    const wstring uncompressed_dir = L"sun_src";
    unordered_set<wstring> cache;
private:
    //get rt.jar files list and put them into a file 'rt.list'
    bool getjarlist(const std::wstring& rtjar_pos) const;

public:
    JarLister();
    bool find_file(const std::wstring& classname){
        if(cache.find(classname)!=cache.end()){
            return true;
        } else{
            bool result = rjd.find_file(StringSplitter(classname));
            if(result){
                cache.insert(classname);
            }
            return result;
        }
    }
    wstring get_sun_dir(){return uncompressed_dir;}
    void print(){rjd.print();}
};

#endif //AUTOMAN_JVM_JARLISTER_H
