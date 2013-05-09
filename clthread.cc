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
			//ReadyPromise.set_value(1);
			ClpInvocation Invocation{new ClpAction{SearchPromise,ReadyPromise,CmdResult}};
			Invocation.RunCode(CodeName,static_cast<char*>(P),Len,vector<string>{"-std=c++11","-c"});
			SearchPromise.set_value(0);
			},
			ref(SearchPromise),ref(ReadyPromise),ref(CmdResult)
		};
	CompileThread.detach();
	return true;
}

bool ClvCompile::SearchAST(){
	if(!ReadyResult.valid()||!SearchResult.valid())
		return false;
	if(ReadyResult.wait_for(chrono::milliseconds(200))!=future_status::ready)
		return false;
	std::cout << "ready\n";
	ReadyResult.get();
	CmdPromise.set_value(1);

	//SearchResult.wait();
	std::cout << SearchResult.get() << "Searchd\n";
		
}
ClvCompile clast;
