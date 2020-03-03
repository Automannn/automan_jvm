//
// Created by 14394 on 2020/2/24.
//

#include "jarlister.h"
#include "utils/utils.h"

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <string>

using std::wcout;
using std::wcerr;
using std::endl;
using std::ifstream;
using std::wstringstream;
using std::string;


using std::make_shared;

// suppose we have only Mac, Linux or Windows
#if (defined (__APPLE__) || defined (__linux__))
wstring splitter(L"/");
#else
//todo: 注意，这个路径从 rt.jar解压出来后，实际上 分隔符 还是 /
wstring splitter(L"/");
#endif

string getProgramDir(){
    char  exeFullPath[MAX_PATH];
    string strPath;
    GetModuleFileName(NULL, exeFullPath, MAX_PATH);
    strPath = exeFullPath;
    int pos  = strPath.find_last_of('\\',strPath.length());
    return strPath.substr(0,pos);
}

/*===--------------- Comparator -----------------===*/
struct shared_RtJarDirectory_compare
{
    using is_transparent = void;		// C++ 14	// TODO: 虽然我并不理解这个到底是什么意思...... // https://stackoverflow.com/questions/32610446/find-a-value-in-a-set-of-shared-ptr

    bool operator() (const shared_ptr<RtJarDirectory> & lhs, const shared_ptr<RtJarDirectory> & rhs) const
    {
        return *lhs < *rhs;
    }

    bool operator() (const shared_ptr<RtJarDirectory> & lhs, const RtJarDirectory & rhs) const
    {
        return *lhs < rhs;
    }

    bool operator() (const RtJarDirectory & lhs, const shared_ptr<RtJarDirectory> & rhs) const
    {
        return lhs < *rhs;
    }

    bool operator() (const shared_ptr<RtJarDirectory> & lhs, const wstring & rhs) const
    {
        return *lhs < rhs;
    }

    bool operator() (const wstring & lhs, const shared_ptr<RtJarDirectory> & rhs) const
    {
        return lhs < *rhs;
    }
};

/*===---------------- RtJarDirectory --------------------*/
RtJarDirectory::RtJarDirectory(const wstring & filename) : name(filename) {
    //equals to ends_with();
    if (filename.find(L".class",filename.length()-7)!= -1)	subdir = nullptr;
    else subdir.reset(new set<shared_ptr<RtJarDirectory>, shared_RtJarDirectory_compare>);	// why shared_ptr cancelled the openator = ... emmmm...
}

shared_ptr<RtJarDirectory> RtJarDirectory::findFolderInThis(const wstring & fdr_name) const	// fdr --> folder
{
    if (this->subdir == nullptr) return nullptr;

    auto iter = this->subdir->find(make_shared<RtJarDirectory>(fdr_name));		// the total `shared_RtJarDirectory_compare` and lots of `operator <` only works for here......
    if (iter != this->subdir->end()) return *iter;
    else return nullptr;
}

void RtJarDirectory::add_file(StringSplitter&& ss)
{
    if (ss.counter() == 0) {		// 仅仅在第一次的时候做检查，看文件到底存不存在
        if (this->find_file(std::move(ss)) == true) return;
        else ss.counter() = 0;
    }

    const wstring& target = ss.result()[ss.counter()];
    if (ss.counter() == ss.result().size() - 1) {	// next will be the target, add.
        subdir->insert(make_shared<RtJarDirectory>(target));
    } else {	// dir.
        auto next_dir = findFolderInThis(target);
        ss.counter() += 1;
        if (next_dir != nullptr) {
            (*next_dir).add_file(std::move(ss));	// delegate to the next level dir.
        } else {	// no next_dir, we should create.
            // this level's `subdir` can't be nullptr :)
            subdir->insert(make_shared<RtJarDirectory>(target));
            next_dir = findFolderInThis(target);
            assert(next_dir != nullptr);
            (*next_dir).add_file(std::move(ss));
        }
    }
}

bool RtJarDirectory::find_file(StringSplitter&& ss) const
{
    const wstring & target = ss.result()[ss.counter()];
    auto next_dir = findFolderInThis(target);
    if (next_dir != nullptr) {
        ss.counter() += 1;
        if (ss.counter() == ss.result().size())	return true;
        else return (*next_dir).find_file(std::move(ss));
    } else
        return false;
}

void RtJarDirectory::print() const
{
#ifdef DEBUG
    std::wcout << "*********************************" << endl;
	wcout.imbue(std::locale(""));
	this->print(0);
	std::wcout << "*********************************" << endl;
#endif
}

void RtJarDirectory::print(int level) const
{
    for(int i = 0; i < level; i ++)	std::wcout << "\t";
    std::wcout << this->name << endl;
    if (this->subdir != nullptr) {
        for(auto & sub : *subdir) {
            (*sub).print(level+1);
        }
    }
}

/*===---------------- Filter ----------------------*/
const unordered_set<wstring> exclude_files{ L"META-INF/" };

/*===---------------- JarLister --------------------*/
bool JarLister::getjarlist(const wstring & rtjar_pos) const
{
    wstringstream cmd;
    //todo: windows路径中包含空格，需要用单引号引起来  即 将 rt.jar的内容 保存在rt.list中
    cmd << L"jar tf " << rtjar_pos << L" > " << this->rtlist;
    int status =  system(wstring_to_utf8(cmd.str()).c_str());
    if (status == -1) {
        exit(-1);
    }
    // TODO: judge whether mkdir is exist?
    if (0==access(wstring_to_utf8(uncompressed_dir).c_str(),F_OK)) {	// 如果存在
        //todo:这里再调试时，发现存在逻辑问题，暂时注释掉. 这一步骤时解压rt.jar的，正常情况下，解压成功后，是可以跳过这个步骤的
        //另外，这个步骤也比较费时间，所以这个检查很有必要
        return true;
    }
    cmd.str(L"");
    cmd << L"mkdir " << uncompressed_dir;
    status = system(wstring_to_utf8(cmd.str()).c_str());
    if (status == -1) {
        exit(-1);
    }
    cmd.str(L"");
    std::wcout << "unzipping rt.jar from: [" << rtjar_pos << "] ... please wait.\n";
    //todo: 因为 windows环境 不支持 控制台字符串的重定向，因此删除重定向命令
    cmd << L"unzip " << rtjar_pos << L" -d " << uncompressed_dir;
    status = system(wstring_to_utf8(cmd.str()).c_str());
    if (status == -1) {  	// http://blog.csdn.net/cheyo/article/details/6595955 [shell 命令是否执行成功的判定]
        std::cerr << "system error!" << endl;
        exit(-1);
    } else {
            if (0 ==status) {
                std::wcout << "unzipping succeed.\n";
                return true;
            }
            else {
                std::cerr << "Your rt.jar file is not right!" << endl;
            }
    }
    return false;
}

wstring pwd;

//todo: 修改 rjd 为windows 的路径
JarLister::JarLister() : rjd(L"root")
{
    pwd = utf8_to_wstring(getProgramDir());
    wstring rtjar_folder;
#if (defined (__APPLE__))
    rtjar_folder = utf8_to_wstring(pt.get<std::string>("path.mac"));
#elif (defined (__linux__))
    rtjar_folder = utf8_to_wstring(pt.get<std::string>("path.linux"));
#else
    //todo: 这里配置 windows的 rt路径
    rtjar_folder = utf8_to_wstring("C:\\Program Files\\Java\\jdk1.8.0_161\\jre\\lib\\");
#endif
    //todo: 这里很重要，需要在外部加上单引号
    rtjar_pos =L"\""+ rtjar_folder + L"rt.jar"+L"\"";

    // copy lib/currency.data to ./lib/currency.data ......
    wstringstream ss;
    int status = system(wstring_to_utf8(ss.str()).c_str());
    if (status == -1) {  	// http://blog.csdn.net/cheyo/article/details/6595955 [shell 命令是否执行成功的判定]
        std::cerr << "system error!" << endl;
    }

    bool success = this->getjarlist(rtjar_pos);
    if (!success)	exit(-1);

    ifstream f(wstring_to_utf8(this->rtlist), std::ios_base::in);
    std::string s;
    while(!f.eof()) {
        f >> s;		// 这里有一个细节。因为最后一行仅仅有个回车，所以会读入空，也就是 s 还是原来的 s，即最后一个名字被读入了两遍。使用其他的方法对效率不好，因此在 add_file 中解决了。如果检测到有，忽略。
        if (!Filter::filt(utf8_to_wstring(s))) {
            this->rjd.add_file(StringSplitter(utf8_to_wstring(s)));
        }
    }
}




