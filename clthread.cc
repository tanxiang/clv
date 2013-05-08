#include <iostream>
#include "clthread.h"
#include "clangparse.h"

using namespace std;
ClvCompile::ClvCompile():SearchResult{SearchPromise.get_future()},
	ReadyResult{ReadyPromise.get_future()},CmdResult{CmdPromise.get_future()}{}

void* ClvCompile::PchCode(const char* CodeName,void* P,int Len){
	return nullptr;
}

bool ClvCompile::ParseCode(const char* CodeName,void* P,int Len,void* Pch){
	CompileThread = thread{
		[=](promise<int>& SearchPromise,promise<int>& ReadyPromise,std::future<int>& CmdResult){
			ReadyPromise.set_value(1);
			ClpInvocation Invocation{new ClpAction{SearchPromise,ReadyPromise,CmdResult}};
			Invocation.RunCode(CodeName,static_cast<char*>(P),Len,vector<string>{"-std=c++11","-c"});
			ReadyPromise.set_value(0);
			},
			ref(SearchPromise),ref(ReadyPromise),ref(CmdResult)
		};
	CompileThread.detach();
	return true;
}

bool ClvCompile::SearchAST(){
	if(!Ready())
		return false;
	return true;
}
ClvCompile clast;
